
#include "PlayersAPI.h"
#include "GStreamerCore.h"
#include "GraphicsInclude.h"

extern "C" EXPORT_API void* mray_gst_createNetworkPlayer()
{
	GStreamerCore* c = GStreamerCore::Instance();
	if (c)
	{
		GstNetworkVideoPlayer* player = new GstNetworkVideoPlayer();
		return player;
	}
	return 0;
}
extern "C" EXPORT_API void mray_gst_netPlayerSetIP(GstNetworkVideoPlayer* p, const char* ip, int videoPort, bool rtcp)
{
	if (p != nullptr)
	{
		p->SetIPAddress(ip, videoPort, rtcp);
	}
}

extern "C" EXPORT_API void mray_gst_PlayerDestroy(IGStreamerPlayer* p)
{

	if (p != nullptr)
	{
		p->Close();
		delete p;
	}
}

extern "C" EXPORT_API bool mray_gst_netPlayerCreateStream(GstNetworkVideoPlayer* p)
{
	if (p != nullptr)
	{
		return p->CreateStream();
	}
	return false;

}
extern "C" EXPORT_API bool mray_gst_netPlayerGrabFrame(GstNetworkVideoPlayer* p, int &w, int &h)
{
	if (p != nullptr)
	{
		if (p->GrabFrame())
		{
			Vector2d sz = p->GetFrameSize();
			w = sz.x;
			h = sz.y;
			return true;
		}
		return false;
	}
	return false;

}
extern "C" EXPORT_API void mray_gst_netPlayerGetFrameSize(GstNetworkVideoPlayer* p, int &w, int &h)
{
	if (p != nullptr)
	{
		Vector2d sz = p->GetFrameSize();
		w = sz.x;
		h = sz.y;
	}else
		w = h = 0;
}

void CopyToTexture(const ImageInfo* src, uchar* dst)
{
	int len = src->Size.x*src->Size.y * 3;
	uchar* ptr = dst;
	uchar* srcPtr = src->imageData;
	for (int i = 0; i < len; i+=3)
	{
		ptr[0] = srcPtr[0];
		ptr[1] = srcPtr[1];
		ptr[2] = srcPtr[2];
		ptr[3] = 1;
		ptr += 4;
		srcPtr += 3;
	}
}

extern "C" EXPORT_API void mray_gst_netPlayerBlitImage(GstNetworkVideoPlayer* p, void* _TextureNativePtr, int _UnityTextureWidth, int _UnityTextureHeight)
{
	if (p == nullptr || !_TextureNativePtr)
		return;

	const video::ImageInfo* ifo= p->GetLastFrame();

	if (ifo)
	{

#if SUPPORT_D3D9
		// D3D9 case
		if (g_GraphicsDeviceType == kGfxRendererD3D9)
		{
			// Update native texture from code
			if (_TextureNativePtr)
			{
				IDirect3DTexture9* d3dtex = (IDirect3DTexture9*)_TextureNativePtr;
				D3DSURFACE_DESC desc;
				d3dtex->GetLevelDesc(0, &desc);
				D3DLOCKED_RECT lr;
				d3dtex->LockRect(0, &lr, nullptr, 0);

				//uchar* data = new uchar[desc.Width*desc.Height * 4];
				
				//memcpy((unsigned char*)lr.pBits, ifo->imageData, desc.Width*desc.Height * 3);
				CopyToTexture(ifo, (uchar*)lr.pBits);

				d3dtex->UnlockRect(0);
				//delete [] data;
			}
		}
#endif

#if SUPPORT_D3D11
		// D3D11 case
		if (g_GraphicsDeviceType == kGfxRendererD3D11)
		{
			ID3D11DeviceContext* ctx = nullptr;
			g_D3D11GraphicsDevice->GetImmediateContext(&ctx);

			// update native texture from code
			if (_TextureNativePtr)
			{
				ID3D11Texture2D* d3dtex = (ID3D11Texture2D*)_TextureNativePtr;
				D3D11_TEXTURE2D_DESC desc;
				d3dtex->GetDesc(&desc);
				//ctx->UpdateSubresource(d3dtex, 0, nullptr, ifo->imageData, desc.Width * 3, 0);

				uchar* data = new uchar[desc.Width*desc.Height * 4];

				CopyToTexture(ifo, (uchar*)data);
				ctx->UpdateSubresource(d3dtex, 0, nullptr, data, desc.Width * 4, 0);
				delete [] data;

			}

			ctx->Release();
		}
#endif


#if SUPPORT_OPENGL
		// OpenGL case
		if (g_GraphicsDeviceType == kGfxRendererOpenGL)
		{
			// update native texture from code
			if (_TextureNativePtr)
			{
				GLuint gltex = (GLuint)(size_t)(_TextureNativePtr);
				glBindTexture(GL_TEXTURE_2D, gltex);
				int texWidth, texHeight;
				glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
				glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texWidth, texHeight, GL_RGB, GL_UNSIGNED_BYTE, ifo->imageData);

			}
		}
#endif
	}
}



extern "C" EXPORT_API void mray_gst_PlayerPlay(IGStreamerPlayer* p)
{
	if (p != nullptr)
	{
		 p->Play();
	}

}
extern "C" EXPORT_API void mray_gst_PlayerPause(IGStreamerPlayer* p)
{
	if (p != nullptr)
	{
		p->Pause();
	}


}
extern "C" EXPORT_API void mray_gst_PlayerStop(IGStreamerPlayer* p)
{

	if (p != nullptr)
	{
		p->Stop();
	}
}
extern "C" EXPORT_API bool mray_gst_PlayerIsLoaded(IGStreamerPlayer* p)
{
	if (p != nullptr)
	{
		return p->IsLoaded();
	}
	return false;

}
extern "C" EXPORT_API bool mray_gst_PlayerIsPlaying(IGStreamerPlayer* p)
{
	if (p != nullptr)
	{
		return p->IsPlaying();
	}
	return false;
}
extern "C" EXPORT_API void mray_gst_PlayerClose(IGStreamerPlayer* p)
{
	if (p != nullptr)
	{
		p->Close();
	}

}

