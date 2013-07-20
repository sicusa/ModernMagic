#include "stdafx.h"

// 如果需要引入fcyStream则必须在f2d之前引入
#include <fcyIO/fcyStream.h>
#include <fcyFile/fcyModelMeshFile.h>
#include <fcyFile/fcyModelMeshExporter.h>
#include <fcyOS/fcyDebug.h>
#include <fcyOS/fcyMultiThread.h>

// UI库
#include <fcyUIBase/fuiPage.h>
#include <fcyUIBase/fuiFactory.h>
#include <fcyUIControl/fuiLabel.h>

// f2d头文件
#include <f2d.h>

#include <iostream>
#include <string>

#include <ModernMagic.h>

using namespace std;
using namespace ModernMagic;

class MyApp :
	public f2dEngineEventListener
{
private:
	// 必要组件
	fcyRefPointer<f2dEngine> m_pEngine;
	f2dFileSys* m_pFileSys;
	f2dRenderer* m_pRenderer;
	f2dRenderDevice* m_pDev;
	f2dSoundSys* m_pSoundSys;
	f2dInputSys* m_pInputSys;
	f2dVideoSys* m_pVideoSys;

	// 渲染器
	fcyRefPointer<f2dGraphics2D> m_pGraph2D;
	fcyRefPointer<f2dGraphics3D> m_pGraph3D;

	// 键盘
	fcyRefPointer<f2dInputKeyboard> m_KeyBoard;

	// 纹理
	fcyRefPointer<f2dTexture2D> m_texture;
	fcyRefPointer<f2dSprite>    m_sprite;

	// 测试用
	fuiResProviderImpl tProvider;
	fcyRefPointer<fuiPage> m_pRootUIPage;

	Bullet *m_role;

	fuiLabel *m_label;

	bool m_paused;

private:  // 功能函数
	void showSelfInfo()
	{
		f2dCPUInfo tInfo;

		m_pEngine->GetCPUInfo(tInfo);

		cout<<"f2d Version"<<(F2DVERSION>>16)<<"."<<(F2DVERSION & 0x0000FFFF)<<"\n"<<endl;

		cout<<"[ CPU制造商 ] "<<tInfo.CPUString<<endl;
		cout<<"[  CPU品牌  ] "<<tInfo.CPUBrandString<<"\n"<<endl;

		cout<<"[ 显卡名称 ] "<<m_pDev->GetDeviceName()<<"\n"<<endl;

		for(fuInt i = 0; i<m_pDev->GetSupportResolutionCount(); i++)
		{
			fcyVec2 t = m_pDev->EnumSupportResolution(i);
			cout<<"[ 分辨率 ] "<<(int)t.x<<"x"<<(int)t.y;

			cout<<" ( 抗锯齿等级 ";
			for(int j = F2DAALEVEL_NONE; j != F2DAALEVEL_16; j++)
			{
				if(m_pDev->CheckMultiSample((F2DAALEVEL)j, true))
				{
					switch(j)
					{
					case F2DAALEVEL_NONE:
						cout<<"x0 ";
						break;
					case F2DAALEVEL_2:
						cout<<"x2 ";
						break;
					case F2DAALEVEL_4:
						cout<<"x4 ";
						break;
					case F2DAALEVEL_8:
						cout<<"x8 ";
						break;
					case F2DAALEVEL_16:
						cout<<"x16 ";
						break;
					}
				}
			}
			cout<<" )"<<endl;
		}

		cout<<endl;
	}
protected: // 引擎消息
	fBool OnUpdate(fDouble ElapsedTime, f2dFPSController* pFPSController, f2dMsgPump* pMsgPump)
	{
		// 消息处理
		f2dMsg tMsg;
		while(FCYOK(pMsgPump->GetMsg(&tMsg)))
		{
			switch(tMsg.Type)
			{
			case F2DMSG_WINDOW_ONCLOSE:
				m_pEngine->Abort();
				break;
			case F2DMSG_WINDOW_ONLOSTFOCUS:
				m_paused = true;
				break;
			case F2DMSG_WINDOW_ONGETFOCUS:
				m_paused = false;
				break;
			default:
				m_pRootUIPage->DealF2DMsg(tMsg);
				break;
			}
		}

		return true;
	}

	fBool OnRender(fDouble ElapsedTime, f2dFPSController* pFPSController)
	{
		if (m_paused) {
			return true;
		}

		// 更新UI
		m_pRootUIPage->Update(ElapsedTime);

		static fDouble last = 0.0;
		last += ElapsedTime;

		if (last > 1.0) {
			m_label->SetProperty(
				L"Text",
				to_wstring(theMMBodyUpdater.GetObjects().size())
				+ L", FPS=" + to_wstring(pFPSController->GetFPS())
				);
			last = 0.0;
		}

		theMMEngine.Update(ElapsedTime);

		// 作图
		m_pDev->Clear(0xFF000000);

		m_pGraph2D->Begin();

		for (auto each : theMMBodyUpdater.GetObjects()) {
			auto body = static_cast<Body*>(each);
			auto &pos = body->GetPosition();
			auto &color = body->GetColor();

			m_sprite->SetColor(fcyColor(body->GetOpacity(), color.R, color.G, color.B));

			fcyVec2 scale(1.0f, 1.0f);

			if (body->IsBullet()) {
				auto rr = static_cast<Bullet*>(body)->GetRenderRadius();
				scale.x = scale.y = rr;
			}
			m_sprite->Draw(m_pGraph2D, *reinterpret_cast<const fcyVec2*>(&pos), scale, body->GetRadian());
		}

		m_pGraph2D->End();

		// 绘制UI
		m_pRootUIPage->Render();
		return true; 
	}
public:
	MyApp()
	{
		m_paused = false;

		m_role = theMMBodyUpdater.Add(new Bullet());
		m_role->SetPosition(512.f, 384.f);

		auto emitter = theMMActionUpdater.Add(new AnnularEmitter());

		auto probody = Object::MakeShared(theMMClassFactory.CreateObject<Bullet>("Bullet"));

		theMMPropertyHelper.SetPropertyByString(probody.get(), "Speed",    "100.0");
		theMMPropertyHelper.SetPropertyByString(probody.get(), "Color",    "0 0 255");
		theMMPropertyHelper.SetPropertyByString(probody.get(), "Position", "512 384");
		theMMPropertyHelper.SetPropertyByString(probody.get(), "Opacity",  "0.5");
		theMMPropertyHelper.SetPropertyByString(probody.get(), "Radius",   "22");
		theMMPropertyHelper.SetPropertyByString(probody.get(), "RenderRadius", "1");
		theMMPropertyHelper.SetPropertyByString(probody.get(), "IsDestoryWhenTimelineEnd", "true");

		auto btl = probody->AppliedTimeline();
		btl->Add(new ActionSleep(3.5f));

		auto ag = btl->Add(new ActionGroup());
		ag->Add(new AnimateTo<float>("RenderRadius", 2.0f, 0.5f, IF_ExponentialOut));
		ag->Add(new AnimateTo<float>("Opacity", 0.0f, 0.5f, IF_ExponentialOut));

		emitter->SetPrototype(probody);
		emitter->SetWayNumber(10);
		emitter->SetInterval(0.1f);
		emitter->SetEmittedNumber(-1);

		auto emitterBodyTL = theMMActionUpdater.Add(new LoopTimeline(-1));
		emitterBodyTL->SetBindingObject(probody.get());

		emitterBodyTL->Add(new AnimateBy<float>("Angle", -360, 8.0f, IF_Smooth));
		emitterBodyTL->Add(new AnimateBy<float>("Angle", 360, 8.0f, IF_Smooth));

		auto emitterBodyTL2 = theMMActionUpdater.Add(new LoopTimeline(-1));
		emitterBodyTL2->SetBindingObject(probody.get());

		emitterBodyTL2->Add(new AnimateTo<Color>("Color", Colors::Red, 2.0f, IF_Smooth));
		emitterBodyTL2->Add(new AnimateTo<Color>("Color", Colors::Blue, 2.0f, IF_Smooth));
		emitterBodyTL2->Add(new AnimateTo<Color>("Color", Colors::Green, 2.0f, IF_Smooth));

		auto emitter2 = theMMActionUpdater.Add(new BodyEmitter());

		emitter2->SetPrototype(probody);
		emitter2->SetOnBodyCreated([](BodyEmitter *sender, Body *body) {
			body->SetSpeed(MathUtil::RandomFloatInRange(100.f, 200.f));

			static const Color colors[] = {
				Colors::Red, Colors::Green, Colors::Blue, Colors::BeachSand, Colors::Cyan, Colors::PureGreen,
				Colors::PureYellowGreen, Colors::DesertSand, Colors::Magenta,
				Colors::Yellow
			};

			body->SetColor(colors[MathUtil::RandomInt(sizeof(colors) / sizeof(Color))]);
			body->SetOpacity(MathUtil::RandomFloat(0.8f));
			body->SetAngle(MathUtil::RandomFloat(360.0f));
		});
		emitter2->SetInterval(0.03f);
		emitter2->SetEmittedNumber(-1);
		/*
		Segment segs[] = {
		Segment(0,  0,  1024,  0),
		Segment(0,  0,  0, 768),
		Segment(1024,  768,  1024, 0),
		Segment(1024,  768,  0, 768)
		};

		for (auto &each : segs) {
		ReboundBoard *rb = new ReboundBoard(each);
		theMMActionUpdater.Add(rb);
		}*/

		theMMEngine.SetWorldBox(BoundingBox(Vector2(-50, -50), Vector2(1024+50, 768+50)));
		theMMEngine.Start();

		struct : public f2dInitialErrListener {
			void OnErr(fuInt TimeTick, fcStr Src, fcStr Desc)
			{
				fChar pBuffer[1024];
				sprintf_s(pBuffer, "初始化失败！\n时间戳：%u\n问题来源：%s\n错误描述：%s", TimeTick, Src, Desc);
				MessageBoxA(0, pBuffer, "f2d 初始化失败", MB_ICONERROR);
			}
		} tErrCallBack;

		// 创建引擎
		if(FCYFAILED(CreateF2DEngineAndInit(
			F2DVERSION,
			fcyRect(50.f, 50.f, 1024.f + 50.f, 768.f + 50.f),
			L"ModernMagic",
			true,
			true,  // VSYNC
			F2DAALEVEL_2,
			this,
			&m_pEngine,
			&tErrCallBack)))
		{ return; }

		// 获得组件
		m_pFileSys = m_pEngine->GetFileSys();
		m_pRenderer = m_pEngine->GetRenderer();
		m_pDev = m_pEngine->GetRenderer()->GetDevice();
		m_pSoundSys = m_pEngine->GetSoundSys();
		m_pInputSys = m_pEngine->GetInputSys();
		m_pVideoSys = m_pEngine->GetVideoSys();

		// 信息自举
		showSelfInfo();

		// 加载资源
		{
			// 映射本地文件夹DemoRes到节点Res
			m_pFileSys->LoadRealPath(L"Res", L"DemoRes\\");
			m_pFileSys->LoadRealPath(L"Simple", L"DemoRes\\Simple\\");

			// 创建渲染器
			m_pDev->CreateGraphics2D(0, 0, &m_pGraph2D);

			m_pDev->CreateGraphics3D(NULL, &m_pGraph3D);
			m_pGraph3D->SetWorldTransform(fcyMatrix4::GetScaleMatrix(0.8f));
			m_pGraph3D->SetProjTransform(fcyMatrix4::GetPespctiveLH(4.f/3.f, 3.14f/4.f, 0.1f, 1000.f));
			m_pGraph3D->SetViewTransform(fcyMatrix4::GetLookAtLH(fcyVec3(0.f,0.f,100.f), fcyVec3(), fcyVec3(0.f,1.f,0.f)));

			// 创建键盘
			m_pInputSys->CreateKeyboard(-1, false, &m_KeyBoard);

			tProvider = fuiResProviderImpl(m_pFileSys, m_pRenderer);

			try
			{
				m_pRootUIPage.DirectSet(new fuiPage(L"Main", m_pRenderer, m_pGraph2D));

				m_pRootUIPage->GetControlStyle()->LoadResFromFile(m_pFileSys->GetStream(L"Simple\\Style.xml"), &tProvider);
				m_pRootUIPage->LoadLayoutFromFile(m_pFileSys->GetStream(L"Simple\\Layout.xml"));
				m_pRootUIPage->SetDebugMode(false);
				m_label = static_cast<fuiLabel*>(m_pRootUIPage->FindControl(L"TestLabel"));

				auto result = m_pDev->CreateTextureFromStream (
					m_pFileSys->GetStream(L"Res\\Images\\bulblu.png"),
					0, 0, false, true, &m_texture
					);
				if (FCYFAILED(result))
					return;

				m_pRenderer->CreateSprite2D(m_texture, &m_sprite);
			}
			catch (const fcyException& e)
			{
				fChar pBuffer[1024];
				sprintf_s(pBuffer, "初始化失败！\n时间戳：%u\n问题来源：%s\n错误描述：%s", e.GetTime(), e.GetSrc(), e.GetDesc());
				MessageBoxA(0, pBuffer, "f2d 初始化失败", MB_ICONERROR);

				return;
			}
		}

		// 显示窗口
		m_pEngine->GetMainWindow()->SetVisiable(true);

		// 隐藏鼠标
		m_pEngine->GetMainWindow()->HideMouse(false);

		// 启动
		m_pEngine->Run(F2DENGTHREADMODE_MULTITHREAD);
	}
	~MyApp()
	{
	}
};

//int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int)
int main()
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	// _CrtSetBreakAlloc(5351);
#endif
	MyApp tApp;

	// system("pause");
	return 0;
}
