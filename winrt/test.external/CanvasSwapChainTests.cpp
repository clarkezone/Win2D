// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using Platform::String;
using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::WRL::Wrappers;
using namespace WinRTDirectX;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Storage::Streams;
using namespace Windows::UI;

TEST_CLASS(CanvasSwapChainTests)
{
    TEST_METHOD(CanvasSwapChain_NativeInterop)
    {
        auto canvasDevice = ref new CanvasDevice();

        auto canvasSwapChain = ref new CanvasSwapChain(canvasDevice, 1, 2, DEFAULT_DPI);

        auto dxgiSwapChain = GetWrappedResource<IDXGISwapChain2>(canvasSwapChain);

        DXGI_SWAP_CHAIN_DESC1 swapChainDescription;
        ThrowIfFailed(dxgiSwapChain->GetDesc1(&swapChainDescription));
        Assert::AreEqual(1u, swapChainDescription.Width);
        Assert::AreEqual(2u, swapChainDescription.Height);

        auto newCanvasSwapChain = GetOrCreate<CanvasSwapChain>(canvasDevice, dxgiSwapChain.Get(), DEFAULT_DPI);
        auto newDxgiSwapChain = GetWrappedResource<IDXGISwapChain2>(newCanvasSwapChain);

        Assert::AreEqual(canvasSwapChain, newCanvasSwapChain);
        Assert::AreEqual(dxgiSwapChain.Get(), newDxgiSwapChain.Get());

        ExpectCOMException(E_INVALIDARG, L"Existing resource wrapper has a different DPI.",
            [&]
            {
                GetOrCreate<CanvasSwapChain>(canvasDevice, dxgiSwapChain.Get(), DEFAULT_DPI + 3);
            });
    }

    TEST_METHOD(CanvasSwapChain_DrawOperation)
    {
        auto canvasDevice = ref new CanvasDevice();

        auto canvasSwapChain = ref new CanvasSwapChain(canvasDevice, 1, 1, DEFAULT_DPI);

        Color referenceColor = Colors::Red;

        auto drawingSession = canvasSwapChain->CreateDrawingSession(referenceColor);
        delete drawingSession;

        auto dxgiSwapChain = GetWrappedResource<IDXGISwapChain2>(canvasSwapChain);

        ComPtr<IDXGISurface> dxgiSwapChainSurface;
        ThrowIfFailed(dxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiSwapChainSurface)));

        auto wrappedSwapChainSurface = CreateDirect3DSurface(dxgiSwapChainSurface.Get());

        auto swapChainRenderTarget = CanvasRenderTarget::CreateFromDirect3D11Surface(canvasDevice, wrappedSwapChainSurface);

        auto color = swapChainRenderTarget->GetPixelColors();

        Assert::AreEqual(1u, color->Length);
        Assert::AreEqual(referenceColor, color[0]);
    }

    TEST_METHOD(CanvasSwapChain_PropagatesDpiToDrawingSession)
    {
        const float dpi = 145;

        auto canvasDevice = ref new CanvasDevice();
        auto canvasSwapChain = ref new CanvasSwapChain(canvasDevice, 1, 2, dpi);
        auto drawingSession = canvasSwapChain->CreateDrawingSession(Colors::Black);

        Assert::AreEqual(dpi, drawingSession->Dpi);
    }

    TEST_METHOD(CanvasSwapChain_ResizeBuffersPreservesRotationAndTransformButResetsSourceSize)
    {
        auto device = ref new CanvasDevice();
        auto swapChain = ref new CanvasSwapChain(device, 256, 256, DEFAULT_DPI);

        // Validate the initial property values.
        Assert::AreEqual(Size{ 256, 256 }, swapChain->Size);
        Assert::AreEqual(BitmapSize{ 256, 256 }, swapChain->SizeInPixels);
        Assert::AreEqual(CanvasSwapChainRotation::None, swapChain->Rotation);
        Assert::AreEqual<float3x2>(float3x2{ 1, 0, 0, 1, 0, 0 }, swapChain->TransformMatrix);
        Assert::AreEqual(Size{ 256, 256 }, swapChain->SourceSize);

        // Change the rotation, transform, and source size.
        swapChain->Rotation = CanvasSwapChainRotation::Rotate270;
        swapChain->TransformMatrix = float3x2{ 2, 0, 0, 3, 4, 5 };
        swapChain->SourceSize = Size{ 123, 234 };

        // Read back the modified values.
        Assert::AreEqual(CanvasSwapChainRotation::Rotate270, swapChain->Rotation);
        Assert::AreEqual<float3x2>(float3x2{ 2, 0, 0, 3, 4, 5 }, swapChain->TransformMatrix);
        Assert::AreEqual(Size{ 123, 234 }, swapChain->SourceSize);

        // Resize the swapchain.
        swapChain->ResizeBuffers(257, 257);

        // This should preserve rotation and transform, but reset the SourceSize.
        Assert::AreEqual(Size{ 257, 257 }, swapChain->Size);
        Assert::AreEqual(BitmapSize{ 257, 257 }, swapChain->SizeInPixels);
        Assert::AreEqual(CanvasSwapChainRotation::Rotate270, swapChain->Rotation);
        Assert::AreEqual<float3x2>(float3x2{ 2, 0, 0, 3, 4, 5 }, swapChain->TransformMatrix);
        Assert::AreEqual(Size{ 257, 257 }, swapChain->SourceSize);
    }

    TEST_METHOD(CanvasSwapChain_WaitForVBlank_HW)
    {
        auto device = ref new CanvasDevice();
        auto swapChain = ref new CanvasSwapChain(device, 1, 1, DEFAULT_DPI);

        // This should wait for vblank on the primary output.
        swapChain->WaitForVerticalBlank();
    }

    TEST_METHOD(CanvasSwapChain_WaitForVBlank_SW)
    {
        auto device = ref new CanvasDevice(true); // Force software renderer
        auto swapChain = ref new CanvasSwapChain(device, 1, 1, DEFAULT_DPI);

        // This should be a no-op and silently succeed.
        swapChain->WaitForVerticalBlank();
    }

    TEST_METHOD(CanvasSwapChain_Constructors)
    {
        auto creator = ref new StubResourceCreatorWithDpi(ref new CanvasDevice());
        
        auto swapChain1 = ref new CanvasSwapChain(creator, 23, 42);
        auto swapChain2 = ref new CanvasSwapChain(creator, Size{ 7, 21 });

        Assert::AreEqual(swapChain1->Size, Size{ 23, 42 });
        Assert::AreEqual(swapChain2->Size, Size{ 7, 21 });
    }
};
