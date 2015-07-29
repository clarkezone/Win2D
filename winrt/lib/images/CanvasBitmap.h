// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "PolymorphicBitmapmanager.h"
#include "TextureUtilities.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace WinRTDirectX;

    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas::Effects;
    using namespace ABI::Windows::Foundation;
    using namespace ABI::Windows::Storage::Streams;
    using namespace ABI::Windows::Storage;

    class CanvasBitmapManager;

    class ICanvasBitmapResourceCreationAdapter
    {
    public:
        virtual ~ICanvasBitmapResourceCreationAdapter() = default;

        virtual ComPtr<IWICBitmapSource> CreateWICFormatConverter(HSTRING fileName) = 0;
        virtual ComPtr<IWICBitmapSource> CreateWICFormatConverter(IStream* fileStream) = 0;

        virtual void SaveLockedMemoryToFile(
            HSTRING fileName,
            CanvasBitmapFileFormat fileFormat,
            float quality,
            unsigned int width,
            unsigned int height,
            float dpiX,
            float dpiY,
            ScopedBitmapMappedPixelAccess* bitmapLock) = 0;

        virtual void SaveLockedMemoryToStream(
            IRandomAccessStream* stream,
            CanvasBitmapFileFormat fileFormat,
            float quality,
            unsigned int width,
            unsigned int height,
            float dpiX,
            float dpiY,
            ScopedBitmapMappedPixelAccess* bitmapLock) = 0;
    };
    

    [uuid(4684FA78-C721-4531-8CCE-BEA927F95E5D)]
    class ICanvasBitmapInternal : public IUnknown
    {
    public:
        virtual ComPtr<ID2D1Bitmap1> const& GetD2DBitmap() = 0;
    };

    class ICanvasBitmapAdapter
    {
    public:
        virtual ~ICanvasBitmapAdapter() = default;

        virtual ComPtr<IRandomAccessStreamReference> CreateRandomAccessStreamFromUri(IUriRuntimeClass* uri) = 0;
        virtual ComPtr<IAsyncOperation<StorageFile*>> GetFileFromPathAsync(HSTRING path) = 0;
    };

    class CanvasBitmapFactory :
        public ActivationFactory<
            ICanvasBitmapFactory, 
            ICanvasBitmapStatics,
            CloakedIid<ICanvasDeviceResourceFactoryNative>>,
        public PerApplicationPolymorphicBitmapManager
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasBitmap, BaseTrust);

    public:
        //
        // ICanvasBitmapStatics
        //

        IFACEMETHOD(CreateFromDirect3D11Surface)(
            ICanvasResourceCreator* resourceCreator,
            IDirect3DSurface* surface,
            ICanvasBitmap** canvasBitmap) override;

        IFACEMETHOD(CreateFromDirect3D11SurfaceWithDpi)(
            ICanvasResourceCreator* resourceCreator,
            IDirect3DSurface* surface,
            float dpi,
            ICanvasBitmap** canvasBitmap) override;

        IFACEMETHOD(CreateFromDirect3D11SurfaceWithDpiAndAlpha)(
            ICanvasResourceCreator* resourceCreator,
            IDirect3DSurface* surface,
            float dpi,
            CanvasAlphaMode alpha,
            ICanvasBitmap** canvasBitmap) override;

        IFACEMETHOD(CreateFromBytes)(
            ICanvasResourceCreator* resourceCreator,
            uint32_t byteCount,
            BYTE* bytes,
            int32_t widthInPixels,
            int32_t heightInPixels,
            DirectXPixelFormat format,
            ICanvasBitmap** canvasBitmap) override;

        IFACEMETHOD(CreateFromBytesWithDpi)(
            ICanvasResourceCreator* resourceCreator,
            uint32_t byteCount,
            BYTE* bytes,
            int32_t widthInPixels,
            int32_t heightInPixels,
            DirectXPixelFormat format,
            float dpi,
            ICanvasBitmap** canvasBitmap) override;

        IFACEMETHOD(CreateFromBytesWithDpiAndAlpha)(
            ICanvasResourceCreator* resourceCreator,
            uint32_t byteCount,
            BYTE* bytes,
            int32_t widthInPixels,
            int32_t heightInPixels,
            DirectXPixelFormat format,
            float dpi,
            CanvasAlphaMode alpha,
            ICanvasBitmap** canvasBitmap) override;

        IFACEMETHOD(CreateFromColors)(
            ICanvasResourceCreator* resourceCreator,
            uint32_t colorCount,
            ABI::Windows::UI::Color* colors,
            int32_t widthInPixels,
            int32_t heightInPixels,
            ICanvasBitmap** canvasBitmap) override;

        IFACEMETHOD(CreateFromColorsWithDpi)(
            ICanvasResourceCreator* resourceCreator,
            uint32_t colorCount,
            ABI::Windows::UI::Color* colors,
            int32_t widthInPixels,
            int32_t heightInPixels,
            float dpi,
            ICanvasBitmap** canvasBitmap) override;

        IFACEMETHOD(CreateFromColorsWithDpiAndAlpha)(
            ICanvasResourceCreator* resourceCreator,
            uint32_t colorCount,
            ABI::Windows::UI::Color* colors,
            int32_t widthInPixels,
            int32_t heightInPixels,
            float dpi,
            CanvasAlphaMode alpha,
            ICanvasBitmap** canvasBitmap) override;

        IFACEMETHOD(LoadAsyncFromHstring)(
            ICanvasResourceCreator* resourceCreator,
            HSTRING fileName,
            ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation) override;

        IFACEMETHOD(LoadAsyncFromHstringWithDpi)(
            ICanvasResourceCreator* resourceCreator,
            HSTRING fileName,
            float dpi,
            ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation) override;

        IFACEMETHOD(LoadAsyncFromHstringWithDpiAndAlpha)(
            ICanvasResourceCreator* resourceCreator,
            HSTRING fileName,
            float dpi,
            CanvasAlphaMode alpha,
            ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation) override;

        IFACEMETHOD(LoadAsyncFromUri)(
            ICanvasResourceCreator* resourceCreator,
            ABI::Windows::Foundation::IUriRuntimeClass* uri,
            ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation) override;

        IFACEMETHOD(LoadAsyncFromUriWithDpi)(
            ICanvasResourceCreator* resourceCreator,
            ABI::Windows::Foundation::IUriRuntimeClass* uri,
            float dpi,
            ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation) override;

        IFACEMETHOD(LoadAsyncFromUriWithDpiAndAlpha)(
            ICanvasResourceCreator* resourceCreator,
            ABI::Windows::Foundation::IUriRuntimeClass* uri,
            float dpi,
            CanvasAlphaMode alpha,
            ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation) override;

        IFACEMETHOD(LoadAsyncFromStream)(
            ICanvasResourceCreator* resourceCreator,
            IRandomAccessStream* stream,
            ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation) override;

        IFACEMETHOD(LoadAsyncFromStreamWithDpi)(
            ICanvasResourceCreator* resourceCreator,
            IRandomAccessStream* stream,
            float dpi,
            ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation) override;

        IFACEMETHOD(LoadAsyncFromStreamWithDpiAndAlpha)(
            ICanvasResourceCreator* resourceCreator,
            IRandomAccessStream* stream,
            float dpi,
            CanvasAlphaMode alpha,
            ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation) override;

        //
        // ICanvasDeviceResourceFactoryNative
        //
        
        IFACEMETHOD(GetOrCreate)(
            ICanvasDevice* device,
            IUnknown* resource,
            IInspectable** wrapper) override;
    };

    void GetPixelBytesImpl(
        ComPtr<ID2D1Bitmap1> const& d2dBitmap,
        D2D1_RECT_U const& subRectangle,
        uint32_t* valueCount,
        uint8_t** valueElements);

    void GetPixelColorsImpl(
        ComPtr<ID2D1Bitmap1> const& d2dBitmap,
        D2D1_RECT_U const& subRectangle,
        uint32_t* valueCount,
        Color **valueElements);

    void SaveBitmapToFileImpl(
        ComPtr<ID2D1Bitmap1> const& d2dBitmap,
        ICanvasBitmapResourceCreationAdapter* adapter,
        HSTRING rawfileName,
        CanvasBitmapFileFormat fileFormat,
        float quality,
        IAsyncAction **resultAsyncAction);

    void SaveBitmapToStreamImpl(
        ComPtr<ID2D1Bitmap1> const& d2dBitmap,
        ICanvasBitmapResourceCreationAdapter* adapter,
        ComPtr<IRandomAccessStream> const& stream,
        CanvasBitmapFileFormat fileFormat,
        float quality,
        IAsyncAction **resultAsyncAction);

    void SetPixelBytesImpl(
        ComPtr<ID2D1Bitmap1> const& d2dBitmap,
        D2D1_RECT_U const& subRectangle,
        uint32_t valueCount,
        uint8_t* valueElements);

    void SetPixelColorsImpl(
        ComPtr<ID2D1Bitmap1> const& d2dBitmap,
        D2D1_RECT_U const& subRectangle,
        uint32_t valueCount,
        Color *valueElements);

    HRESULT CopyPixelsFromBitmapImpl(
        ICanvasBitmap* to,
        ICanvasBitmap* from,
        int32_t* destX = nullptr,
        int32_t* destY = nullptr,
        int32_t* sourceRectLeft = nullptr,
        int32_t* sourceRectTop = nullptr,
        int32_t* sourceRectWidth = nullptr,
        int32_t* sourceRectHeight = nullptr);

    struct CanvasBitmapTraits
    {
        typedef ID2D1Bitmap1 resource_t;
        typedef CanvasBitmap wrapper_t;
        typedef ICanvasBitmap wrapper_interface_t;
        typedef CanvasBitmapManager manager_t;
    };


    template<typename TRAITS>
    class CanvasBitmapImpl 
        : public Implements<
        RuntimeClassFlags<WinRtClassicComMix>,
        ICanvasBitmap,
        ICanvasImage,
        IGraphicsEffectSource,
        IDirect3DSurface,
        CloakedIid<ICanvasImageInternal>,
        CloakedIid<ICanvasBitmapInternal>,
        CloakedIid<IDirect3DDxgiInterfaceAccess>,
        ChainInterfaces<MixIn<CanvasBitmapImpl<TRAITS>, ResourceWrapper<TRAITS>>, ABI::Windows::Foundation::IClosable, CloakedIid<ICanvasResourceWrapperNative>>>
        , public ResourceWrapper<TRAITS>
    {
        float m_dpi;

    protected:
        ComPtr<ICanvasDevice> m_device;

        CanvasBitmapImpl(
            std::shared_ptr<typename TRAITS::manager_t> manager, 
            ID2D1Bitmap1* resource,
            ICanvasDevice* device)
            : ResourceWrapper(manager, resource)
            , m_device(device)
            , m_dpi(GetDpi(resource))
        {}

    public:
        IFACEMETHODIMP Close() override
        {
            m_device.Reset();
            return ResourceWrapper::Close();
        }

        IFACEMETHODIMP get_SizeInPixels(_Out_ BitmapSize* size) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckInPointer(size);
                    
                    auto& resource = GetResource();
                    D2D1_SIZE_U d2dSize = resource->GetPixelSize();
                    size->Height = d2dSize.height;
                    size->Width = d2dSize.width;
                });
        }
        
        IFACEMETHODIMP get_Size(_Out_ ABI::Windows::Foundation::Size* size) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckInPointer(size);

                    auto& resource = GetResource();
                    D2D1_SIZE_F d2dSize = resource->GetSize();
                    size->Height = d2dSize.height;
                    size->Width = d2dSize.width;
                });
        }

        IFACEMETHODIMP get_Bounds(_Out_ ABI::Windows::Foundation::Rect* bounds) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckInPointer(bounds);

                    auto& resource = GetResource();
                    D2D1_SIZE_F d2dSize = resource->GetSize();
                    bounds->X = 0;
                    bounds->Y = 0;
                    bounds->Width = d2dSize.width;
                    bounds->Height = d2dSize.height;
                });
        }

        IFACEMETHODIMP get_Dpi(float* dpi) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckInPointer(dpi);
                    *dpi = m_dpi;
                });
        }

        IFACEMETHODIMP ConvertPixelsToDips(int pixels, float* dips) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckInPointer(dips);
                    *dips = PixelsToDips(pixels, m_dpi);
                });
        }

        IFACEMETHODIMP ConvertDipsToPixels(float dips, CanvasDpiRounding dpiRounding, int* pixels) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckInPointer(pixels);
                    *pixels = DipsToPixels(dips, m_dpi, dpiRounding);
                });
        }

        IFACEMETHODIMP get_Device(ICanvasDevice** value) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckInPointer(value);
                    ThrowIfFailed(m_device.CopyTo(value));
                });
        }

        IFACEMETHODIMP get_Format(DirectXPixelFormat* value) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckInPointer(value);

                    auto& d2dBitmap = GetResource();
                    auto format = d2dBitmap->GetPixelFormat();
                    *value = static_cast<DirectXPixelFormat>(format.format);
                });
        }

        IFACEMETHODIMP get_AlphaMode(CanvasAlphaMode* value) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckInPointer(value);

                    auto& d2dBitmap = GetResource();
                    auto format = d2dBitmap->GetPixelFormat();
                    *value = FromD2DAlphaMode(format.alphaMode);
                });
        }

        // IDirect3DSurface
        IFACEMETHODIMP get_Description(Direct3DSurfaceDescription* value) override
        {
            return ExceptionBoundary(
                [&]
                {
                    CheckInPointer(value);

                    auto& d2dBitmap = GetResource();
                    ComPtr<IDXGISurface> dxgiSurface;
                    ThrowIfFailed(d2dBitmap->GetSurface(&dxgiSurface));                    
                    ThrowIfFailed(dxgiSurface->GetDesc(ReinterpretAs<DXGI_SURFACE_DESC*>(value)));
                });
        }

        // ICanvasImageInternal
        ComPtr<ID2D1Image> GetD2DImage(ID2D1DeviceContext*) override
        {
            return GetResource();
        }

        ICanvasImageInternal::RealizedEffectNode GetRealizedEffectNode(ID2D1DeviceContext* deviceContext, float targetDpi) override
        {
            UNREFERENCED_PARAMETER(deviceContext);
            UNREFERENCED_PARAMETER(targetDpi);

            return RealizedEffectNode{ GetResource(), m_dpi, 0 };
        }

        // ICanvasBitmapInternal
        virtual ComPtr<ID2D1Bitmap1> const& GetD2DBitmap() override
        {
            return GetResource();
        }

        // IDirect3DDxgiInterfaceAccess
        IFACEMETHODIMP GetInterface(REFIID iid, void** p)
        {
            return ExceptionBoundary(
                [&]
                {
                    auto& d2dBitmap = GetResource();
                    ComPtr<IDXGISurface> dxgiSurface;
                    ThrowIfFailed(d2dBitmap->GetSurface(&dxgiSurface));
                    ThrowIfFailed(dxgiSurface.CopyTo(iid, p));
                });
        }

        IFACEMETHODIMP SaveToFileAsync(
            HSTRING fileName,
            IAsyncAction **resultAsyncAction) override
        {
            return SaveToFileWithBitmapFileFormatAndQualityAsync(
                fileName,
                CanvasBitmapFileFormat::Auto,
                DEFAULT_CANVASBITMAP_QUALITY,
                resultAsyncAction);
        }        

        IFACEMETHODIMP SaveToFileWithBitmapFileFormatAsync(
            HSTRING fileName,
            CanvasBitmapFileFormat fileFormat,
            IAsyncAction **resultAsyncAction) override
        {
            return SaveToFileWithBitmapFileFormatAndQualityAsync(
                fileName,
                fileFormat,
                DEFAULT_CANVASBITMAP_QUALITY,
                resultAsyncAction);
        }

        IFACEMETHODIMP SaveToFileWithBitmapFileFormatAndQualityAsync(
            HSTRING rawfileName,
            CanvasBitmapFileFormat fileFormat,
            float quality,
            IAsyncAction **resultAsyncAction) override
        {
            return ExceptionBoundary(
                [=]
                {
                    CheckInPointer(rawfileName);
                    CheckAndClearOutPointer(resultAsyncAction);

                    auto& d2dBitmap = GetResource();

                    SaveBitmapToFileImpl(
                        d2dBitmap.Get(), 
                        Manager()->GetAdapter(),
                        rawfileName, 
                        fileFormat,
                        quality,
                        resultAsyncAction);
                });
        }

        IFACEMETHODIMP SaveToStreamAsync(
            IRandomAccessStream* stream,
            CanvasBitmapFileFormat fileFormat,
            IAsyncAction** asyncAction) override
        {
            return SaveToStreamWithQualityAsync(
                stream,
                fileFormat,
                DEFAULT_CANVASBITMAP_QUALITY,
                asyncAction);
        }

        IFACEMETHODIMP SaveToStreamWithQualityAsync(
            IRandomAccessStream* stream,
            CanvasBitmapFileFormat fileFormat,
            float quality,
            IAsyncAction** asyncAction) override
        {
            return ExceptionBoundary(
                [=]
                {
                    CheckInPointer(stream);
                    CheckAndClearOutPointer(asyncAction);

                    auto& d2dBitmap = GetResource();

                    SaveBitmapToStreamImpl(
                        d2dBitmap.Get(), 
                        Manager()->GetAdapter(),
                        stream,
                        fileFormat,
                        quality,
                        asyncAction);
                });
        }

        IFACEMETHODIMP GetPixelBytes(
            uint32_t* valueCount,
            uint8_t** valueElements) override
        {
            return ExceptionBoundary(
                [&]
                {
                    auto& d2dBitmap = GetResource();

                    GetPixelBytesImpl(
                        d2dBitmap,
                        GetResourceBitmapExtents(d2dBitmap),
                        valueCount, 
                        valueElements);
                });
        }

        IFACEMETHODIMP GetPixelBytesWithSubrectangle(
            int32_t left,
            int32_t top,
            int32_t width,
            int32_t height,
            uint32_t* valueCount,
            uint8_t** valueElements) override
        {
            return ExceptionBoundary(
                [&]
                {
                    auto& d2dBitmap = GetResource();

                    GetPixelBytesImpl(
                        d2dBitmap,
                        ToD2DRectU(left, top, width, height),
                        valueCount, 
                        valueElements);
                });
        }

        IFACEMETHODIMP GetPixelColors(
            uint32_t* valueCount,
            ABI::Windows::UI::Color **valueElements) override
        {
            return ExceptionBoundary(
                [&]
                {
                    auto& d2dBitmap = GetResource();

                    GetPixelColorsImpl(
                        d2dBitmap,
                        GetResourceBitmapExtents(d2dBitmap),
                        valueCount, 
                        valueElements);
                });
        }

        IFACEMETHODIMP GetPixelColorsWithSubrectangle(
            int32_t left,
            int32_t top,
            int32_t width,
            int32_t height,
            uint32_t* valueCount,
            ABI::Windows::UI::Color **valueElements) override
        {
            return ExceptionBoundary(
                [&]
                {
                    auto& d2dBitmap = GetResource();

                    GetPixelColorsImpl(
                        d2dBitmap,
                        ToD2DRectU(left, top, width, height),
                        valueCount, 
                        valueElements);
                });
        }

        IFACEMETHODIMP SetPixelBytes(
            uint32_t valueCount,
            uint8_t* valueElements) override
        {
            return ExceptionBoundary(
                [&]
                {
                    auto& d2dBitmap = GetResource();

                    SetPixelBytesImpl(
                        d2dBitmap,
                        GetResourceBitmapExtents(d2dBitmap),
                        valueCount, 
                        valueElements);
                });
        }

        IFACEMETHODIMP SetPixelBytesWithSubrectangle(
            uint32_t valueCount,
            uint8_t* valueElements,
            int32_t left,
            int32_t top,
            int32_t width,
            int32_t height) override
        {
            return ExceptionBoundary(
                [&]
                {
                    auto& d2dBitmap = GetResource();

                    SetPixelBytesImpl(
                        d2dBitmap,
                        ToD2DRectU(left, top, width, height),
                        valueCount, 
                        valueElements);
                });
        }

        IFACEMETHODIMP SetPixelColors(
            uint32_t valueCount,
            ABI::Windows::UI::Color* valueElements) override
        {
            return ExceptionBoundary(
                [&]
                {
                    auto& d2dBitmap = GetResource();

                    SetPixelColorsImpl(
                        d2dBitmap,
                        GetResourceBitmapExtents(d2dBitmap),
                        valueCount, 
                        valueElements);
                });
        }

        IFACEMETHODIMP SetPixelColorsWithSubrectangle(
            uint32_t valueCount,
            ABI::Windows::UI::Color* valueElements,
            int32_t left,
            int32_t top,
            int32_t width,
            int32_t height) override
        {
            return ExceptionBoundary(
                [&]
                {
                    auto& d2dBitmap = GetResource();

                    SetPixelColorsImpl(
                        d2dBitmap,
                        ToD2DRectU(left, top, width, height),
                        valueCount, 
                        valueElements);
                });
        }

        IFACEMETHODIMP GetBounds(
            ICanvasDrawingSession *drawingSession,
            Rect *bounds) override
        {
            return GetImageBoundsImpl(this, drawingSession, nullptr, bounds);
        }

        IFACEMETHODIMP GetBoundsWithTransform(
            ICanvasDrawingSession *drawingSession,
            Numerics::Matrix3x2 transform,
            Rect *bounds) override
        {
            return GetImageBoundsImpl(this, drawingSession, &transform, bounds);
        }

        IFACEMETHODIMP CopyPixelsFromBitmap(
            ICanvasBitmap* otherBitmap)
        {
            return CopyPixelsFromBitmapImpl(
                this,
                otherBitmap);
        }

        IFACEMETHODIMP CopyPixelsFromBitmapWithDestPoint(
            ICanvasBitmap* otherBitmap,
            int32_t destX,
            int32_t destY)
        {
            return CopyPixelsFromBitmapImpl(
                this,
                otherBitmap,
                &destX,
                &destY);
        }

        IFACEMETHODIMP CopyPixelsFromBitmapWithDestPointAndSourceRect(
            ICanvasBitmap* otherBitmap,
            int32_t destX,
            int32_t destY,
            int32_t sourceRectLeft,
            int32_t sourceRectTop,
            int32_t sourceRectWidth,
            int32_t sourceRectHeight)
        {      
            return CopyPixelsFromBitmapImpl(
                this, 
                otherBitmap, 
                &destX, 
                &destY, 
                &sourceRectLeft, 
                &sourceRectTop, 
                &sourceRectWidth, 
                &sourceRectHeight);
        }

    private:

        D2D1_RECT_U GetResourceBitmapExtents(ComPtr<ID2D1Bitmap1> const d2dBitmap)
        {
            const D2D1_SIZE_U size = d2dBitmap->GetPixelSize();
            return D2D1::RectU(0, 0, size.width, size.height);
        }
    };


    class CanvasBitmap :
        public RuntimeClass<                                    
            RuntimeClassFlags<WinRtClassicComMix>,              
            MixIn<CanvasBitmap, CanvasBitmapImpl<CanvasBitmapTraits>>>
        , public CanvasBitmapImpl<CanvasBitmapTraits>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasBitmap, BaseTrust);

    public:
        CanvasBitmap(
            std::shared_ptr<CanvasBitmapManager> manager,
            ID2D1Bitmap1* bitmap,
            ICanvasDevice* device);
    };


    class CanvasBitmapManager : public ResourceManager<CanvasBitmapTraits>
    {
        std::shared_ptr<ICanvasBitmapResourceCreationAdapter> m_adapter;

    public:
        CanvasBitmapManager(std::shared_ptr<ICanvasBitmapResourceCreationAdapter> adapter);

        ComPtr<CanvasBitmap> CreateNew(
            ICanvasDevice* canvasDevice, 
            HSTRING fileName,
            float dpi,
            CanvasAlphaMode alpha);

        ComPtr<CanvasBitmap> CreateNew(
            ICanvasDevice* canvasDevice,
            IStream* fileStream,
            float dpi,
            CanvasAlphaMode alpha);

        ComPtr<CanvasBitmap> CreateNew(
            ICanvasDevice* device,
            uint32_t byteCount,
            BYTE* bytes,
            int32_t widthInPixels,
            int32_t heightInPixels,
            DirectXPixelFormat format,
            float dpi,
            CanvasAlphaMode alpha);

        ComPtr<CanvasBitmap> CreateNew(
            ICanvasDevice* device,
            uint32_t colorCount,
            Color* colors,
            int32_t widthInPixels,
            int32_t heightInPixels,
            float dpi,
            CanvasAlphaMode alpha);

        ComPtr<CanvasBitmap> CreateWrapper(
            ICanvasDevice* device,
            ID2D1Bitmap1* bitmap);

        ICanvasBitmapResourceCreationAdapter* GetAdapter();
    };
}}}}
