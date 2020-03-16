﻿/******************************************************************

	DXUI界面库 - 示例程序 v200316
	作者: YZ
	QQ: 2441987560
	WeChat: yangzhen2441987560
	Email: 2441987560@qq.com
	Github: https://github.com/IDXGI/DXUI-Libaray

	此库来之不易，使用请标明作者。商业用途请先联系我。
	共同维护良好的开源环境！

*******************************************************************/

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN// 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <Windows.h>
// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "resource.h"//程序资源文件
#include <commdlg.h>//更换背景图的文件浏览框使用
#include <psapi.h>//获取内存占用信息使用


//*************以上是Windows程序基础头文件
#include "DXUI_Library.h"//**********DXUI界面库***********

//动画的贝塞尔曲线参数
DBezier b = { 0.19f, 1.0f, 0.22f, 1.0f };

DWindow Main;
DWindow About;

DButton Close;
DButton MinSize;
DButton SetBackImg;
DButton BAbout;

DButton About_OK;

DLabel DXUI;
DLabel good;
DLabel good1;

DLabel gitlb;
DLabel picfrom;
DLabel CPURate;
DLabel MemInfo;

DLabel my;

DTextBox tb;
DTextBox tb1;
DTextBox github;

DImageBox ib;

HINSTANCE hInst;//当前实例

//主窗口的参数定义
#define Title       L"DXUI Demo"//标题栏文本
#define WindowClass L"DXUI Class"//主窗口类名
#define WindowWidth  1200
#define WindowHeight 700

//关于窗口的长宽定义
#define AboutWidth 400
#define AboutHeight 500


//获取性能信息的相关函数
#define UpdateDelay 300//更新信息间隔

void __stdcall DConProc(HWND hWnd, UINT id, DControlType type, DControlMsg msg, WPARAM wParam, LPARAM lParam);

void __cdecl UpdateInformationThread(void* param);
unsigned long long FileTimeSub(FILETIME ftEndTime, FILETIME ftStartTime);
UINT GetCPUUsageRate();
void SetMemoryInfo();
bool UpdateThreadAlive = false;
bool UpdateInfo = true;

// 此代码模块中包含的函数的前向声明:
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	hInst = hInstance;

	//执行应用程序初始化:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return false;
	}

	MSG msg;

	//主消息循环:
	while (GetMessageW(&msg, nullptr, 0, 0))
	{
		if (!TranslateAcceleratorW(msg.hwnd, nullptr, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	//如果刷新CPU占用率的线程没退出，则等待退出
	UpdateInfo = false;
	while (UpdateThreadAlive)
	{
		Sleep(50);
	}

	return (int)msg.wParam;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	//****由于DXUI库全面支持高DPI场景，所以在使用前 应在项目设置里设置高DPI支持，来防止系统缩放 导致模糊****
	Main.hIcon = LoadIconW(hInst, MAKEINTRESOURCE(IDI_DXUI));
	Main.hIconSmall = LoadIconW(hInst, MAKEINTRESOURCE(IDI_SMALL));

	if (!Main.Create(hInst, WindowClass, Title, WndProc,
		(GetSystemMetrics(SM_CXSCREEN) * 96 / Main.GetDpiX() - WindowWidth) / 2,
		(GetSystemMetrics(SM_CYSCREEN) * 96 / Main.GetDpiY() - WindowHeight) / 2,
		WindowWidth, WindowHeight))
		return false;

	//使用步骤：初始化各个控件及窗口 —— 向窗口添加控件 —— 创建渲染线程

	Close.Init({ WindowWidth - 35, 0 }, { 35, 35 },
		//    ↓ Normal状态下的按钮样式(填充色、描边色、文字色、由其它状态转到此状态的动画时长 单位:秒)
		{ { 255, 60, 40, 0.0f }, { 0, 0, 0, 0.0f }, { 255, 255, 255, 0.8f }, 0.25f },
		//    ↓ MouseMove状态下的按钮样式(填充色、描边色、文字色、由其它状态转到此状态的动画时长 单位:秒)
		{ { 255, 70, 50, 0.8f }, { 200, 200, 200, 0.0f }, { 255, 255, 255, 1.0f }, 0.25f },
		//    ↓ Click状态下的按钮样式(填充色、描边色、文字色、由其它状态转到此状态的动画时长 单位:秒)
		{ { 150, 50, 30, 1.0f }, { 150, 150, 200, 0.0f }, { 255, 255, 255, 1.0f }, 0.05f },
		//    ↓ Disable状态下的按钮样式(填充色、描边色、文字色、由其它状态转到此状态的动画时长 单位:秒)
		{ { 0, 0, 0, 1.0f }, { 0, 0, 0, 1.0f }, { 0, 0, 0, 1.0f }, 0.3f },
		0.0f, 0.0f, 0.0f, L"×", L"微软雅黑", 30, DFontWeight::LightWeight);

	MinSize.Init({ WindowWidth - 70, 0 }, { 35, 35 },
		{ { 255, 255, 255, 0.0f }, { 26, 217, 110, 0.0f }, { 255, 255, 255, 0.8f }, 0.25f },
		{ { 255, 255, 255, 0.3f }, { 35, 231, 131, 0.0f }, { 255, 255, 255, 1.0f }, 0.25f },
		{ { 255, 255, 255, 0.5f }, { 31, 207, 108, 1.0f }, { 255, 255, 255, 1.0f }, 0.05f },
		{ { 0, 0, 0, 1.0f }, { 0, 0, 0, 1.0f }, { 0, 0, 0, 1.0f }, 0.3f },
		0.0f, 0.0f, 0.0f, L"—", L"微软雅黑", 13, DFontWeight::BoldWeight);

	SetBackImg.Init({ WindowWidth - 270, WindowHeight - 55 }, { 125, 50 },
		{ { 255, 255, 255, 0.15f }, { 60, 60, 60, 0.3f }, { 255, 255, 255, 0.9f }, 0.25f },
		{ { 255, 255, 255, 0.35f }, { 60, 60, 60, 0.5f }, { 255, 255, 255, 1.0f }, 0.3f },
		{ { 255, 255, 255, 0.65f }, { 30, 30, 30, 1.0f }, { 255, 255, 255, 1.0f }, 0.1f },
		{ { 0, 0, 0, 1.0f }, { 0, 0, 0, 1.0f }, { 0, 0, 0, 1.0f }, 0.3f },
		25.0f, 25.0f, 0.0f, L"设置背景图", L"微软雅黑", 18);

	BAbout.Init({ WindowWidth - 135, WindowHeight - 55 }, { 125, 50 },
		{ { 255, 255, 255, 0.0f }, { 255, 255, 255, 0.0f }, { 255, 255, 255, 1.0f }, 0.3f },
		{ { 255, 255, 255, 0.3f }, { 255, 255, 255, 0.0f }, { 255, 255, 255, 1.0f }, 0.3f },
		{ { 255, 255, 255, 1.0f }, { 255, 255, 255, 0.0f }, { 0, 0, 0, 1.0f }, 0.1f },
		{ { 0, 0, 0, 1.0f }, { 0, 0, 0, 1.0f }, { 0, 0, 0, 1.0f }, 0.3f },
		25, 25, 0.0f, L"About", L"微软雅黑", 23, DFontWeight::NormalWeight);

	About_OK.Init({ AboutWidth / 2 - 50, AboutHeight - 60 }, { 100, 40 },
		{ { 50, 50, 50, 0.0f }, { 50, 50, 50, 1.0f }, { 50, 50, 50, 1.0f }, 0.25f },
		{ { 50, 50, 50, 1.0f }, { 50, 50, 50, 1.0f }, { 255, 255, 255, 1.0f }, 0.25f },
		{ { 100, 100, 100, 1.0f }, { 100, 100, 100, 1.0f }, { 255, 255, 255, 1.0f }, 0.05f },
		{ { 0, 0, 0, 1.0f }, { 0, 0, 0, 1.0f }, { 0, 0, 0, 1.0f }, 0.3f },
		20, 20, 2.0f, L"确定", L"微软雅黑", 22, DFontWeight::LightWeight, DMouseStyle::HandMouse);

	DXUI.Init({ WindowWidth / 2 - 150, 15 }, { 300, 75 },
		{ { 255, 255, 255, 0.85f }, false, false, 0.3f },
		{ { 125, 175, 255, 1.0f }, false, true, 0.3f },
		{ { 120, 32, 255, 1.0f }, false, true, 0.2f },
		false, L"DXUI 界面库", L"微软雅黑", 50, false, DFontWeight::NormalWeight, DAlignment::Center, DAlignment::Center);

	good.Init({ 50, 100 }, { WindowWidth - 100, 300 },
		{ { 255, 255, 255, 0.9f }, false, false, 0.3f },
		{ { 50, 155, 255, 1.0f }, false, false, 0.3f },
		{ { 50, 155, 255, 1.0f }, false, false, 0.1f },
		false, L"取“DXUI”这个名字是因为此界面库使用Direct 2D图形库来绘制，使用Wic库实现加载图片及部分功能，使用DWrite库来绘制处理文字。基于Direct系列的界面库少之又少，一是实现太过复杂，学习成本太高。二是考虑到硬件兼容的问题。而Direct 2D完美的解决了这些问题，同时保留了硬件加速的优点，用来做界面库，实现流畅的动画再好不过了。\nDXUI界面库的优点就是：   1.控件样式可以90%自定义，并且完善的透明度支持为界面增色不少。\n2.基于事件驱动的动画，用户无操作时不消耗资源。\n3.极低的CPU、GPU、内存占用，不用担心办公电脑等低配置环境的运行问题。\n4.局部刷新技术，背景图再大，上层控件动画也能流畅播放。\n5.非常简单，开发者只需要考虑如何找到好看的颜色，仅通过几行代码即可添加控件。\n6.高兼容性，无需考虑硬件支持问题，就算不支持，可无缝切换到软件绘制，效率优于GDI。\n7.拥有比GDI/GDI+更高的绘图质量，界面更清晰大方。\n8.完善的高DPI支持，再也不用担心系统缩放模糊的问题。",
		L"微软雅黑", 21, true, DFontWeight::NormalWeight, DAlignment::Center, DAlignment::Center);

	good1.Init({ 570, 410 }, { 590, 100 },
		{ { 255, 255, 255, 0.85f }, false, false, 0.25f },
		{ { 66, 158, 30, 1.0f }, false, false, 0.25f },
		{ { 66, 158, 30, 1.0f }, false, false, 0.1f },
		false, L"目前有4种控件：按钮、标签、全自绘编辑框、图片框\n以后有时间准备增加控件页、列表等复杂控件", L"微软雅黑", 25, true, DFontWeight::NormalWeight, DAlignment::Center, DAlignment::Center);

	picfrom.Init({ WindowWidth - 495, WindowHeight - 27 }, { 220, 20 },
		{ { 255, 255, 255, 0.6f }, false, false, 0.25f },
		{ { 255, 255, 255, 1.0f }, false, true, 0.25f },
		{ { 255, 255, 255, 1.0f }, false, true, 0.1f },
		false, L"感谢图片来源：QQ1924286463", L"微软雅黑", 15, true, DFontWeight::NormalWeight, DAlignment::Center, DAlignment::Center);

	CPURate.Init({ 15, WindowHeight - 27 }, { 170, 20 },
		{ { 255, 255, 255, 0.7f }, false, false, 0.25f },
		{ { 255, 255, 255, 1.0f }, false, false, 0.25f },
		{ { 255, 255, 255, 1.0f }, false, false, 0.1f },
		false, L"系统CPU使用率：0 %", L"微软雅黑", 15, false, DFontWeight::NormalWeight, DAlignment::Near, DAlignment::Center);

	MemInfo.Init({ 187, WindowHeight - 27 }, { 450, 20 },
		{ { 255, 255, 255, 0.7f }, false, false, 0.25f },
		{ { 255, 255, 255, 1.0f }, false, false, 0.25f },
		{ { 255, 255, 255, 1.0f }, false, false, 0.1f },
		false, L"进程使用内存：0.0 MB | 峰值使用内存: 0.0 MB (不准)", L"微软雅黑", 15, false, DFontWeight::NormalWeight, DAlignment::Near, DAlignment::Center);

	gitlb.Init({ 570, 530 }, { 70, 40 },
		{ { 255, 255, 255, 0.85f }, false, false, 0.25f },
		{ { 255, 255, 255, 1.0f }, false, false, 0.25f },
		{ { 255, 255, 255, 1.0f }, false, false, 0.1f },
		false, L"Github:", L"微软雅黑", 20, true, DFontWeight::NormalWeight, DAlignment::Center, DAlignment::Center);

	github.Init({ 650, 530 }, { 400, 38 },
		{ {240, 240, 240, 0.2f }, { 150, 150, 150, 1.0f }, {255, 255, 255, 1.0f}, 0.25f },
		{ {240, 240, 240, 0.2f }, {255, 255, 255, 1.0f}, {21, 182, 255, 1.0f}, 0.25f },
		{ {255, 255, 255, 0.1f }, {255, 255, 255, 1.0f}, {21, 182, 255, 1.0f}, 0.1f },
		{ {240, 240, 240, 1.0f }, {255, 255, 255, 1.0f}, {0, 0, 0, 1.0f}, 0.3f },
		5.0f, 5.0f, 1.5f, L"微软雅黑", 20, false, false, true, DFontWeight::NormalWeight, DAlignment::Center, DAlignment::Center,
		{ 255, 255, 255, 1.0f }, 1.5f, { 255, 255, 255, 0.2f });

	github.SetText(L"https://github.com/IDXGI/DXUI-Libaray");//文本框被设为只读，但SetText可以设置，用户无法修改

	my.Init({ 20, 0 }, { AboutWidth - 40, AboutHeight - 150 },
		{ { 0, 0, 0, 1.0f }, false, false, 0.25f },
		{ { 66, 158, 30, 1.0f }, false, false, 0.25f },
		{ { 66, 158, 30, 1.0f }, false, false, 0.1f },
		false, L"杨X，山东济南，2019级高一，会点C/C++，大概学了5年了，平时喜欢编程。\n\n此库来之不易，使用请标明作者。商业用途请先联系我。\nQQ: 2441987560\nWeChat: yangzhen2441987560\nEmail: 2441987560@qq.com\n\n欢迎通过各种方式提出宝贵的意见，每个周末我都会查看，有不懂的也可以找我交流。",
		L"微软雅黑", 20, true, DFontWeight::BoldWeight, DAlignment::Center, DAlignment::Center);

	tb.Init({ 50, 410 }, { 500, 37 },
		{ {255, 255, 255, 0.5f }, { 150, 150, 150, 1.0f }, { 0, 0, 0, 1.0f }, 0.25f },
		{ {245, 245, 245, 0.4f }, {255, 255, 255, 1.0f}, { 0, 0, 0, 1.0f }, 0.25f },
		{ {240, 240, 240, 0.3f }, {255, 255, 255, 1.0f}, { 0, 0, 0, 1.0f }, 0.15f },
		{ {240, 240, 240, 1.0f }, {255, 255, 255, 1.0f}, {0, 0, 0, 1.0f}, 0.3f },
		18.0f, 18.0f, 1.5f, L"微软雅黑", 20, true, true, false, DFontWeight::NormalWeight, DAlignment::Near, DAlignment::Near, { 255, 255, 255, 1.0f }, 2.0f, { 255, 255, 255, 0.3f });
	tb1.Init({ 70, 470 }, { 400, 37 },
		{ {255, 255, 255, 0.5f }, { 150, 150, 150, 1.0f }, { 0, 0, 0, 1.0f }, 0.25f },
		{ {245, 245, 245, 0.4f }, {255, 255, 255, 1.0f}, { 0, 0, 0, 1.0f }, 0.25f },
		{ {240, 240, 240, 0.3f }, {255, 255, 255, 1.0f}, { 0, 0, 0, 1.0f }, 0.15f },
		{ {240, 240, 240, 1.0f }, {255, 255, 255, 1.0f}, {0, 0, 0, 1.0f}, 0.3f },
		18.0f, 18.0f, 1.5f, L"微软雅黑", 20, true, true, false, DFontWeight::NormalWeight, DAlignment::Near, DAlignment::Near, { 255, 255, 255, 1.0f }, 2.0f, { 255, 255, 255, 0.3f });

	tb.SetText(L"4线程高斯模糊，在此处按Enter");
	tb1.SetText(L"控件套娃哈哈哈");
	// 图片框（从资源加载），ResourceID为图片资源ID，TypeName为添加资源时设置的TypeName。
	//Module为当前模块ID，默认为null即当前程序（资源在Dll中时，Module应为Dll的Module）
	//CanMoveWindow为是否可以拖动此控件来移动主窗口
	//ScaleMode为图片是 等比例缩放 还是 拉伸以填充
	ib.LoadFromResource({ 0, 0 }, { WindowWidth, WindowHeight }, IDR_PIC1, L"pic", nullptr, true, DImageScaleMode::Fill);

	//主窗口初始化
	Main.Init(DConProc, 4.0f, 0.8f, 60, { 0, 0, 0, 1.0f });

#if _DEBUG
	Main.Dbg_ShowUpdateRect(true);
	About.Dbg_ShowUpdateRect(true);
#endif // _DEBUG


	//添加控件，控件遮盖关系以添加的顺序为依据
	//如果想让某控件的背景为另一个控件，则必须添加背景控件为该控件的Parent，那该控件叫做背景控件的Child
	//Child控件不能超出Parent控件，超出后会以Parent控件的边缘裁剪
	Main.AddControl(&ib);
	Main.AddControl(&Close, ib);
	Main.AddControl(&MinSize, ib);

	Main.AddControl(&SetBackImg, ib);
	Main.AddControl(&BAbout, ib);

	Main.AddControl(&DXUI, ib);
	Main.AddControl(&good, ib);
	Main.AddControl(&good1, ib);
	Main.AddControl(&picfrom, ib);
	Main.AddControl(&CPURate, ib);
	Main.AddControl(&MemInfo, ib);
	Main.AddControl(&gitlb, ib);

	Main.AddControl(&tb, ib);
	Main.AddControl(&tb1, tb);
	Main.AddControl(&github, ib);

	//绘制是在一个独立的线程内进行的（保证了流畅性），这里要创建一个渲染线程，所有控件就开始绘制了
	Main.CreateRenderThread();

	_beginthread(UpdateInformationThread, 0, nullptr);//这是更新左下角性能信息的线程，在里面循环获取信息并设置文本框的内容（DXUI库线程安全，用户可在任意线程调用）

	Main.Show();

	return true;
}

void __stdcall DConProc(HWND hWnd, UINT id, DControlType type, DControlMsg msg, WPARAM wParam, LPARAM lParam)
{
	//用户可以处理控件的消息返回值，来达到响应控件操作的目的

//DWindow类可隐式转换为HWND，这里直接用它与hWnd比较
	if (hWnd == Main)//多窗口时的分辨处理
	{
		switch (msg)//分辨消息，并比较ID判断
		{
		case DControlMsg::Control_LButtonUp://这里响应的按钮的按下操作
		{
			if (id == Close)//关闭按钮被按下
			{
				Main.Release();
			}
			else if (id == MinSize)//最小化按钮被按下
			{
				ShowWindow(hWnd, SW_MINIMIZE);
			}
			else if (id == SetBackImg)//设置背景图按钮被按下，这里弹出文件浏览框并设置图片
			{
				OPENFILENAMEW ofn = { 0 };
				wchar_t szFile[256] = { 0 };
				wchar_t szFileTitle[256] = { 0 };
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hWnd;
				ofn.lpstrFilter = L"图片文件(*.bmp,*.jpg,*.jpe,*.jpeg,*.png,*.gif,*.heic)\0*.bmp;*.jpg;*.jpe;*.jpeg;*.png;*.gif;*.heic;\0全部文件(*.*)\0*.*;\0\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFile = szFile;
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFileTitle = szFileTitle;
				ofn.nMaxFileTitle = sizeof(szFileTitle);
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;
				if (GetOpenFileNameW(&ofn))
				{
					if (ib.LoadFromFile({ 0, 0 }, { WindowWidth, WindowHeight }, szFile, true, DImageScaleMode::Fill))
						Main.DeleteControl(picfrom);
				}
			}
			else if (id == BAbout)//关于 按钮被按下，这里显示关于窗口
			{
				if (About.IsInit())
				{
					MessageBoxW(About, L"您已打开关于窗口！", L"按钮消息", MB_OK);
					break;
				}

				About.Parent = Main;

				if (!About.Create(hInst, WindowClass, Title, WndProc,
					(GetSystemMetrics(SM_CXSCREEN) * 96 / About.GetDpiX() - AboutWidth) / 2,
					(GetSystemMetrics(SM_CYSCREEN) * 96 / About.GetDpiY() - AboutHeight) / 2,
					AboutWidth, AboutHeight))
					return;

				my.SetPosition(20, 0);
				my.SetTotalOpacity(0.0f);

				About.Init(DConProc);
				About.AddControl(&my);//介绍的标签
				About.AddControl(&About_OK);//关于窗口的“确定”键
				About.CreateRenderThread();
				About.Show();

				my.SetPosition(20, 35, 600, &b);
				my.SetTotalOpacity(1.0f, 1500, &b);

				ib.SetTotalOpacity(0.5f, 1000, &b);
			}
			break;
		}
		case DControlMsg::Control_StateChanged:
		{
			if (id == tb)
			{
				DControlState after = (DControlState)lParam;
				if (after == DControlState::MouseMove || after == DControlState::Edit)
				{
					tb.SetSize(500, 250, 600, &b);
					tb.SetRadius(0.0f, 0.0f, 600, &b);
				}
				else if (after == DControlState::Normal)
				{
					tb.SetSize(500, 37, 600, &b);
					tb.SetRadius(18.0f, 18.0f, 600, &b);
				}
			}
			if (id == tb1)
			{
				DControlState after = (DControlState)lParam;
				if (after == DControlState::MouseMove || after == DControlState::Edit)
				{
					tb1.SetSize(400, 150, 600, &b);
					tb1.SetRadius(0.0f, 0.0f, 600, &b);
				}
				else if (after == DControlState::Normal)
				{
					tb1.SetSize(400, 37, 600, &b);
					tb1.SetRadius(18.0f, 18.0f, 600, &b);
				}
			}
			else if (id == SetBackImg)
			{
				DControlState after = (DControlState)lParam;
				if (after == DControlState::MouseMove || after == DControlState::Click)
				{
					SetBackImg.SetRadius(0.0f, 25.0f, 600, &b);
				}
				else if (after == DControlState::Normal)
				{
					SetBackImg.SetRadius(25.0f, 25.0f, 600, &b);
				}
			}
			break;
		}
		case DControlMsg::TextBox_Enter:
		{
			if (id == tb)
			{
				DWORD a = timeGetTime();
				ib.GaussianBlur(5);
				a = timeGetTime() - a;
				UINT wid, hei;
				ib.GetSize(&wid, &hei);

				wchar_t sOut[256];
				_stprintf_s(sOut, L"图片大小: %d×%d  耗时: %d ms", wid, hei, a);
				tb.AddText(sOut);
			}
			break;
		}
		}
	}
	else if (hWnd == About)
	{
		if (msg == DControlMsg::Control_LButtonUp && id == About_OK)
		{
			ib.SetTotalOpacity(1.0f, 1000, &b);

			About.Release();
		}
	}

	return;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//让DXUI处理响应的消息，来改变控件的状态
	LRESULT res = Main.DXUIMessageProc(hWnd, message, wParam, lParam);
	LRESULT res1 = About.DXUIMessageProc(hWnd, message, wParam, lParam);

	//由于DXUI处理的Windows某些消息后，不能让DefWindowProc再处理，所以ReturnValue不为-1时(即表示需要返回一个值)，则把值返回过去。
	if (res != -1)
		return res;
	else if (res1 != -1)//如果返回值不为-1，则返回DXUI库处理后的返回值
		return res1;


	switch (message)
	{
	case WM_DESTROY:
		if (hWnd == Main)//如果是关于窗口销毁了，那就释放关于窗口的类
		{
			//此时为Main窗口销毁时
			if (About.IsInit())//如果About窗口存在，则销毁它
				About.Release();

			PostQuitMessage(0);
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void __cdecl UpdateInformationThread(void* param)
{
	UpdateThreadAlive = true;
	wchar_t sOut[256];
	while (UpdateInfo)
	{
		_stprintf_s(sOut, L"系统CPU使用率：%d %%", GetCPUUsageRate());
		CPURate.SetText(sOut);//设置文本框内容
		SetMemoryInfo();
	}
	UpdateThreadAlive = false;
	return;
}
inline unsigned long long FileTimeSub(FILETIME ftEndTime, FILETIME ftStartTime)
{
	unsigned long long nDeltaTime;

	unsigned long long nEndTime = (unsigned long long)ftEndTime.dwHighDateTime << 32 | ftEndTime.dwLowDateTime;
	unsigned long long nStartTime = (unsigned long long)ftStartTime.dwHighDateTime << 32 | ftStartTime.dwLowDateTime;

	nDeltaTime = nEndTime - nStartTime;

	return nDeltaTime;
}
inline UINT GetCPUUsageRate()
{
	FILETIME ftStartIdleTime, ftStartKernelTime, ftStartUserTime;
	FILETIME ftEndIdleTime, ftEndKernelTime, ftEndUserTime;

	GetSystemTimes(&ftStartIdleTime, &ftStartKernelTime, &ftStartUserTime);
	Sleep(UpdateDelay);
	GetSystemTimes(&ftEndIdleTime, &ftEndKernelTime, &ftEndUserTime);

	unsigned long long nDeltaIdleTime = FileTimeSub(ftEndIdleTime, ftStartIdleTime);
	unsigned long long nDeltaKernelTime = FileTimeSub(ftEndKernelTime, ftStartKernelTime);
	unsigned long long nDeltaUserTime = FileTimeSub(ftEndUserTime, ftStartUserTime);

	if (nDeltaKernelTime + nDeltaUserTime == 0)
	{
		return 0;
	}

	UINT nCPUUsageRate = (UINT)(((nDeltaKernelTime + nDeltaUserTime - nDeltaIdleTime) * 100) / (nDeltaKernelTime + nDeltaUserTime));

	return nCPUUsageRate;
}

inline void SetMemoryInfo()
{
	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc;

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
	if (!hProcess)
		return;

	wchar_t sOut[256];
	if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
	{
		_stprintf_s(sOut, L"进程使用内存：%.2f MB | 峰值使用内存：%.2f MB (不准)", pmc.WorkingSetSize / 1048576.0f, pmc.PeakWorkingSetSize / 1048576.0f);
		MemInfo.SetText(sOut);//设置文本框内容
	}
	else
		MemInfo.SetText(L"进程使用内存：0.0 MB | 峰值使用内存: 0.0 MB (不准)");//设置文本框内容

	CloseHandle(hProcess);
}
