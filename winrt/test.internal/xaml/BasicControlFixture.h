// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "mocks\MockCanvasSwapChain.h"

template<typename TRAITS>
struct BasicControlFixture
{
    typedef typename TestAdapter<TRAITS>::type             adapter_t;
    typedef typename TRAITS::control_t                     control_t;
    typedef typename TRAITS::createResourcesEventHandler_t createResourcesEventHandler_t;
    typedef typename TRAITS::drawEventHandler_t            drawEventHandler_t;

    static int const InitialWidth = 100;
    static int const InitialHeight = 200;

    std::shared_ptr<adapter_t> Adapter;
    ComPtr<control_t> Control;
    ComPtr<StubUserControl> UserControl;

    BasicControlFixture()
    {
    }

    BasicControlFixture(BasicControlFixture const&) = delete;
    BasicControlFixture& operator=(BasicControlFixture const&) = delete;

    virtual ~BasicControlFixture() = default;

    virtual void CreateAdapter()
    {
        Adapter = std::make_shared<adapter_t>();
    }

    void CreateControl()
    {
        Control = Make<control_t>(Adapter);
        UserControl = static_cast<StubUserControl*>(As<IUserControl>(Control).Get());
    }

    void Load()
    {
        UserControl->Resize(Size{static_cast<float>(InitialWidth), static_cast<float>(InitialHeight)});
        ThrowIfFailed(UserControl->LoadedEventSource->InvokeAll(nullptr, nullptr));
    }

    void RaiseUnloadedEvent()
    {
        ThrowIfFailed(UserControl->UnloadedEventSource->InvokeAll(nullptr, nullptr));
    }

    void RenderSingleFrame();

    void PrepareAdapterForRenderingResource();

    void RenderAnyNumberOfFrames()
    {
        int anyNumberOfTimes = 5;
        for (auto i = 0; i < anyNumberOfTimes; ++i)
            RenderSingleFrame();
    }

    EventRegistrationToken AddCreateResourcesHandler(createResourcesEventHandler_t* handler)
    {
        return AddEventHandler(&control_t::add_CreateResources, handler);
    }

    EventRegistrationToken AddDrawHandler(drawEventHandler_t* handler)
    {
        return AddEventHandler(&control_t::add_Draw, handler);
    }

protected:
    template<typename T, typename HANDLER>
    EventRegistrationToken AddEventHandler(HRESULT (STDMETHODCALLTYPE T::* addMethod)(HANDLER*, EventRegistrationToken*), HANDLER* handler)
    {
        EventRegistrationToken token;
        ThrowIfFailed((Control.Get()->*addMethod)(handler, &token));
        return token;
    }
};

inline void BasicControlFixture<CanvasControlTraits>::RenderSingleFrame()
{
    Adapter->RaiseCompositionRenderingEvent();
}


inline void BasicControlFixture<CanvasAnimatedControlTraits>::RenderSingleFrame()
{
    Adapter->Tick();
}


inline void BasicControlFixture<CanvasControlTraits>::PrepareAdapterForRenderingResource()
{
    Adapter->CreateCanvasImageSourceMethod.AllowAnyCall();
}


inline void BasicControlFixture<CanvasAnimatedControlTraits>::PrepareAdapterForRenderingResource()
{
    auto swapChainManager = std::make_shared<MockCanvasSwapChainManager>();

    Adapter->CreateCanvasSwapChainMethod.AllowAnyCall(
        [=](ICanvasDevice*, float, float, float, CanvasAlphaMode)
    {
        auto mockSwapChain = swapChainManager->CreateMock();
        mockSwapChain->CreateDrawingSessionMethod.AllowAnyCall(
            [](Color, ICanvasDrawingSession** value)
        {
            auto ds = Make<MockCanvasDrawingSession>();
            return ds.CopyTo(value);
        });
        mockSwapChain->PresentMethod.AllowAnyCall();

        mockSwapChain->put_TransformMethod.AllowAnyCall();

        return mockSwapChain;
    });
}

struct Static_BasicControlFixture : public BasicControlFixture<CanvasControlTraits>
{
    void RaiseAnyNumberOfSurfaceContentsLostEvents()
    {
        int anyNumberOfTimes = 5;
        for (auto i = 0; i < anyNumberOfTimes; ++i)
            Adapter->RaiseSurfaceContentsLostEvent();
    }    
};

struct Animated_BasicControlFixture : public BasicControlFixture<CanvasAnimatedControlTraits>
{
    template<typename T>
    void TickUntil(T predicate)
    {
        const int maxTickCount = 500;

        int tickCount = 0;

        while (tickCount < maxTickCount && !predicate())
        {
            Adapter->Tick();
            ++tickCount;
        }

        Assert::IsFalse(tickCount == maxTickCount, L"TickUntil predicate never satisfied, even after loads of Ticks");
    }

    void VerifyTickLoopIsStillRunning()
    {
        for (int i = 0; i < 10; ++i)
            Adapter->Tick();

        Assert::IsTrue(Adapter->GameThreadHasPendingWork(), L"Tick loop is running");
        Assert::IsFalse(Adapter->HasPendingActionsOnUiThread(), L"No pending change action");
    }

};

template<typename T>
struct ControlFixture;

template<>
struct ControlFixture<CanvasControlTraits> : public Static_BasicControlFixture
{
    ControlFixture()
    {
        CreateAdapter();
        CreateControl();
    }
};

template<>
struct ControlFixture<CanvasAnimatedControlTraits> : public Animated_BasicControlFixture
{
    ComPtr<MockD2DDeviceContext> DeviceContext;
    ComPtr<StubD2DDevice> D2DDevice;
    ComPtr<StubCanvasDevice> Device;

    ControlFixture()
        : DeviceContext(Make<MockD2DDeviceContext>())
        , D2DDevice(Make<StubD2DDevice>())
        , Device(Make<StubCanvasDevice>(D2DDevice))
    {
        CreateAdapter();
        CreateControl();
    }

    virtual void CreateAdapter() override
    {
        Adapter = CreateAnimatedControlTestAdapter(DeviceContext, D2DDevice, Device);
    }

    EventRegistrationToken AddUpdateHandler(Animated_UpdateEventHandler* handler)
    {
        return AddEventHandler(&CanvasAnimatedControl::add_Update, handler);
    }

    void ExpectOneCreateSwapChain()
    {
        auto swapChainManager = Adapter->SwapChainManager;

        Adapter->CreateCanvasSwapChainMethod.SetExpectedCalls(1,
            [swapChainManager](ICanvasDevice* device, float width, float height, float dpi, CanvasAlphaMode alphaMode)
            {
                return CreateTestSwapChain(swapChainManager, device);
            });
    }

    bool IsChangedActionRunning()
    {
        return Adapter->HasPendingActionsOnUiThread();
    }

    void RaiseLoadedAndVerify()
    {
        Assert::IsFalse(IsChangedActionRunning());
        Load();
        Assert::IsTrue(IsChangedActionRunning());
        Adapter->DoChanged();
        Assert::IsFalse(IsChangedActionRunning());
    }
    
    static
    ComPtr<CanvasSwapChain> CreateTestSwapChain(
        std::shared_ptr<CanvasSwapChainManager> const& swapChainManager,
        ICanvasDevice* device)
    {
        return swapChainManager->Create(
            device,
            1.0f,
            1.0f,
            DEFAULT_DPI,
            PIXEL_FORMAT(B8G8R8A8UIntNormalized),
            2,
            CanvasAlphaMode::Premultiplied);
    }
};


typedef ControlFixture<CanvasControlTraits> CanvasControlFixture;
typedef ControlFixture<CanvasAnimatedControlTraits> CanvasAnimatedControlFixture;


template<class TRAITS>
struct ControlFixtureWithRecreatableDeviceManager : public ControlFixture<TRAITS>
{
    MockRecreatableDeviceManager<TRAITS>* DeviceManager;
    std::function<void(ChangeReason)> ChangedCallback;

    ControlFixtureWithRecreatableDeviceManager()
        : DeviceManager(nullptr)
    {
        CreateAdapter();

        Adapter->CreateRecreatableDeviceManagerMethod.SetExpectedCalls(1,
            [=]
            {
                Assert::IsNull(DeviceManager);
                auto manager = std::make_unique<MockRecreatableDeviceManager<TRAITS>>();
                manager->SetChangedCallbackMethod.SetExpectedCalls(1,
                    [=](std::function<void(ChangeReason)> fn)
                    {
                        ChangedCallback = fn;
                    });

                DeviceManager = manager.get();
                return manager;
            });

        CreateControl();
    }

    void EnsureChangedCallback()
    {
        EnsureChangedCallbackImpl<TRAITS>();
    }

private:

    template<typename T>
    void EnsureChangedCallbackImpl() {}

    template<>
    void EnsureChangedCallbackImpl<CanvasAnimatedControlTraits>()
    {
        Adapter->DoChanged();
    }
};
    
