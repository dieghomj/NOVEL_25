#include <stdio.h>
#include <windows.h> 
//���C�u�����Ǎ�
#pragma comment(lib, "winmm.lib")		//�����Đ��Ŏg�p����
#pragma execution_character_set("utf-8")

//Tool Functions


//===================================================================================

//BGM �֐�
const char* checkBGMStatus(int BGMNo, char* BGMStatus, HWND hWnd)
{
	char tmp[32];
	sprintf_s(tmp, "%s%d%s", "status BGM", BGMNo, " mode");
	mciSendStringA(tmp, BGMStatus, 255, hWnd);
	return BGMStatus;
}
int playBGM(int BGMNo, HWND hWnd)
{
	char tmp[32];
	sprintf_s(tmp, "%s%d%s", "play BGM", BGMNo, " notify");
	mciSendStringA(tmp, nullptr, 0, hWnd);
	return BGMNo;
}
void restartBGM(int BGMNo, HWND hWnd)
{
	char tmp[32];
	sprintf_s(tmp, "%s%d%s", "seek BGM", BGMNo, " to start");
	mciSendStringA(tmp, nullptr, 0, hWnd);
	playBGM(BGMNo, hWnd);
}
void stopBGM(int BGMNo, HWND hWnd)
{
	char tmp[32];
	sprintf_s(tmp, "%s%d%s", "stop BGM", BGMNo, "");
	restartBGM(BGMNo, hWnd);
	mciSendStringA(tmp, nullptr, 0, hWnd);
}


LRESULT CALLBACK WindowProc(
	HWND hWnd,		//Window Handler
	UINT uMsg,		//Message
	WPARAM wParam,	//Width Parameter
	LPARAM lParam)	//Length Parameter
{
	HDC hDC;		//�e�o�C�X�R���e�L�X�g
	PAINTSTRUCT ps; //�`��\����

	static char disptext[128];	//�\���m�F�p

	static HDC hMemDC;		//�������[�f�o�C�X�R���e�L�X�g

	static HFONT hFont;		//�t�H���g�n���h��

	static int sceneNo = 0;		//Scene Number

	static bool dispSel = false;	//�I�����\�ؑ�
	static int selNo = 1;

	static int BGMNo = 1; //BGM�ԍ�

	static char BGMStatus[256]; //BGM�X�e�[�^�X���b�Z�[�W�擾�p
	
	static char sel[2][50 * 2]; //�I��
	static char story[11][50 * 2];

	static HBITMAP hBack[5]; 


	
	//���b�Z�[�W�ʂ̏���
	switch (uMsg)
	{
	case WM_DESTROY:
		//Destroying has to be done in the opposite order to how it was created!!

		//mp3�̃N���[�Y
		mciSendString("close BGM4", nullptr, 0, hWnd);
		mciSendString("close BGM3", nullptr, 0, hWnd);
		mciSendString("close BGM2", nullptr, 0, hWnd);
		mciSendString("close BGM1", nullptr, 0, hWnd);

		//Delete Background Bitmap
		DeleteObject(hBack[sceneNo]);

		//Delete Font
		DeleteObject(hFont);

		//Delete Memory Device Context Handler
		DeleteDC(hMemDC);

		//Informs Window that the application is closing
		PostQuitMessage(0);
		return 0;

	case WM_CREATE:		//�E�B���h�E����������鎞

		//������DC�̍쐬
		hMemDC = CreateCompatibleDC(nullptr);

		hFont = CreateFont(
			60,							//������
			20,							//������
			0,							//�p�x
			0,							//�x�[�X���C���p�x
			FW_REGULAR,					//����
			FALSE,						//
			FALSE,						//����
			FALSE,						//�ŏ���
			SHIFTJIS_CHARSET,			//�����Z�b�g
			OUT_DEFAULT_PRECIS,			//�o�͐��x
			CLIP_DEFAULT_PRECIS,		//�N���b�v���O���x
			DEFAULT_QUALITY,			//�o�͕i��
			VARIABLE_PITCH | FF_ROMAN,	//	���s�b�`�ƃt�H���g�t�@�~��
			"MS Gothic");			//����(nullptr:���ݎg���Ă��鏑��)
		
		//�w�i�̓ǂݍ���
		hBack[0] = (HBITMAP)LoadImage(
			nullptr,					//�C���X�^���X
			"Data\\BMP\\street.bmp",	//�t�@�C����
			IMAGE_BITMAP,				//�r�b�g�}�b�v
			0,0,						//�摜�̂͂΁A�������i�O�Ŏ����ݒ�j
			LR_LOADFROMFILE);			//�t�@�C������ǂݍ���

		hBack[1] = (HBITMAP)LoadImage(
			nullptr,					//�C���X�^���X
			"Data\\BMP\\mansion.bmp",	//�t�@�C����
			IMAGE_BITMAP,				//�r�b�g�}�b�v
			0, 0,						//�摜�̂͂΁A�������i�O�Ŏ����ݒ�j
			LR_LOADFROMFILE);

		hBack[2] = (HBITMAP)LoadImage(
			nullptr,					//�C���X�^���X
			"Data\\BMP\\torii.bmp",	//�t�@�C����
			IMAGE_BITMAP,				//�r�b�g�}�b�vx
			0, 0,						//�摜�̂͂΁A�������i�O�Ŏ����ݒ�j
			LR_LOADFROMFILE);			//�t�@�C������ǂݍ���

		hBack[10] = (HBITMAP)LoadImage(
			nullptr,					//�C���X�^���X
			"Data\\BMP\\heya.bmp",	//�t�@�C����
			IMAGE_BITMAP,				//�r�b�g�}�b�v
			0, 0,					//�摜�̂͂΁A�������i�O�Ŏ����ݒ�j
			LR_LOADFROMFILE);			//�t�@�C������ǂݍ���

		//������̃R�s�[
		strcpy_s(disptext, "�́X�B�B�B");

		//�I�����̃e�L�X�g
		strcpy_s(sel[0], "���ꂢ�ȃW���C�A��");
		strcpy_s(sel[1], "���ʂ̃W���C�A��");

		strcpy_s(story[0], "����ɂ���");
		strcpy_s(story[1], "���Ȃ������Ƃ����̂�");
		strcpy_s(story[2], "�����҂̂��Ȃ��ɂ�\n���ꂢ�ȃW���C�A�����v���Z���g�I");
		strcpy_s(story[10],"?���������Ȃ���\n���ʂ̃W���C�A�����v���Z���g�I�I");


		//MIDI�܂���mp3�t�@�C���̃I�v�V����
		mciSendString("open Data\\BGM\\Holiday.mp3 alias BGM1", nullptr, 0, hWnd);
		mciSendString("open Data\\BGM\\Last.mp3 alias BGM2", nullptr, 0, hWnd);
		mciSendString("open Data\\BGM\\Pinch.mp3 alias BGM3", nullptr, 0, hWnd);
		mciSendString("open Data\\BGM\\ThemeOfGian.mp3 alias BGM4",nullptr,0,hWnd);

		//BGM�̍Đ�
		//BGM�ԍ��ݒ�
		BGMNo = playBGM(1, hWnd);
		//�^�C�}�[�ݒ�
		SetTimer(hWnd,
			1,			//�^�C�}�[No
			100,		//����
			nullptr);	//��莞�Ԃɓ������֐���

	case WM_TIMER:	//�^�C�}�[�œ���

		//�Ȃ̏�Ԃ�
		checkBGMStatus(BGMNo, BGMStatus, hWnd);

		//�Ȃ���܂��Ă��邩
		if (strcmp(BGMStatus, "stopped") == 0)
		{
			restartBGM(BGMNo, hWnd);
		}

		return 0;

	case WM_LBUTTONDOWN:		//MOUSE LEFT CLICK DOWN
	case WM_RBUTTONDOWN:		//MOUSE RIGHT CLICK DOWN
		//if (sceneNo + 1 == 1)dispSel = true;
		//else dispSel = false;
		//sceneNo++;			//Show Scene 1
		//���߂�̍ĕ`��
		InvalidateRect(hWnd, nullptr, FALSE);
		return 0;

	case WM_KEYDOWN:
		
		switch (wParam)
		{
		case VK_UP:
			selNo--;
			if (selNo < 1)
				selNo = 1;
			break;
		case VK_DOWN:
			selNo++;
			if (selNo > 2)
				selNo--;
		case VK_LEFT:
		case VK_RIGHT:
		case VK_ESCAPE:
		case 'A':
		case 'x':
			break;
		
		
		case VK_RETURN:			//ENTER KEY PRESSED
			switch (sceneNo)
			{
			case 0:
				sceneNo = 1;
				dispSel = true;

				//BGM�̒�~
				stopBGM(BGMNo, hWnd);

				//BGM�̍Đ�
				BGMNo = playBGM(2, hWnd);
				break;

			case 1:
				if (selNo == 1)
				{
					sceneNo = 2;
					//wave�t�@�C���̍Đ�
					PlaySound("Data\\WAV\\GianVoice.wav",
						nullptr,
						SND_ASYNC | SND_FILENAME | SND_RING);
						//SND_ASYNC			:
						//SND_FILENAME		:
						//SND_LOOP			:
						// 
					//BGM�̒�~
					stopBGM(BGMNo, hWnd);
					//BGM�̍Đ�
					BGMNo = playBGM(BGMNo,hWnd);
				}
				else
				{
					sceneNo = 10;

					//BGM�̒�~
					stopBGM(BGMNo, hWnd);
					//BGM�̍Đ�
					playBGM(4, hWnd);
					BGMNo = 4;
				}
				
				dispSel = false;
				break;

			case 2:
				sceneNo = 0;

				//wave�Đ���~
				PlaySound(nullptr,nullptr,SND_PURGE);
				
				//BGM�̒�~
				stopBGM(BGMNo, hWnd);
				//BGM�̍Đ�
				BGMNo = playBGM(1,hWnd);
				break;
			case 10:
				sceneNo = 0;
				//BGM�̒�~
				stopBGM(BGMNo, hWnd);
				//BGM�̍Đ�
				BGMNo = playBGM(1,hWnd);

				break;		
			
			}

		}

		InvalidateRect(hWnd, nullptr, FALSE);
		return 0;

	case WM_PAINT :		//�E�B���h�E���X�V���ꂽ��

		SelectObject(hMemDC, hBack[sceneNo]);

		//�`��J�n
		hDC = BeginPaint(hWnd,&ps);

		//�w�i�̉摜��������DC�փR�s�[
		//SelectObject(hMemDC, hBack);
		//SelectObject(hMemDC, hBack1);


		//�w�i�̕\��
		BitBlt(hDC,			//�f�o�C�X�R���e�L�X�g
			0, 0,			//�\���ʒu���A�����W
			640, 480,		//�摜���A������
			hMemDC,			//������DC
			0, 0,			//���摜x,y,���W
			SRCCOPY);		//�R�s�[����

		//SetBkMode(hDC, OPAQUE);
		//SetBkColor(hDC, RGB(0x00, 0x00, 0x00));

		SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, RGB(0x00,0xAA,0x00));
		SelectObject(hDC, hFont);

		RECT rect;		//��`�H���\����		
		//������̕\���͈�
		//�E�B���h�E�̑傫�����擾
		//RECT�\���̂�left��top
		GetClientRect(hWnd, &rect);
		//�\���J�n�ʒu�ݒ�
		rect.left = 10;
		rect.top = 20;
		
		
		//������̕\��
		DrawText(hDC,
			story[sceneNo],		//�\�����镶����
			-1,				//���������w��i-1�őS���j
			&rect,			//�\���͈�
			DT_WORDBREAK);	//�܂�Ԃ�

		//�I�����̕\��
		if (dispSel)
		{
			SetTextColor(hDC, RGB(0xFF, 0x00, 0xFF));
			for (int i = 0; i < 2; i++)
			{

				if (selNo == i + 1)
				{
					SetBkMode(hDC, OPAQUE);
					SetBkColor(hDC, RGB(0xFF, 0x00, 0x00));
				}
				else
					SetBkMode(hDC, TRANSPARENT);
				
				TextOut(hDC,
					50,					//��\�����W
					120 + 60 * i,		//��\�����W
					sel[i],				//��\���镶����
					lstrlen(sel[i]));	//��\���镶����̃T�C�Y
			}
		}

		EndPaint(hWnd, &ps);

		return 0;

	}
	//Give back information to MAIN
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


int WINAPI WinMain(
	_In_ HINSTANCE hInstance,	//�C���X�^���X�ԍ�(�E�B���h�E�̔ԍ�)
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PSTR lpCmdLine,
	_In_ int nCmdShow)
{
	WNDCLASS wc;				//Window Class �E�B���h�E�N���X�\����(Struct	ure)
	HWND hWnd;					//Window Handler �E�B���h�E�n���h��
	MSG msg;					//���b�Z�[�W

	//--------------------------------------------
	//�E�B���h�E�N���X�o�^
	//--------------------------------------------
	//�X�^�C���̓o�^
	//�@CS_HREDRAW Horizontal Redraw	�F	���������̍ĕ`��
	//	CS_VREDRAW Vertical Redraw	�F	���������̍ĕ`��
	//

	wc.style = CS_HREDRAW | CS_VREDRAW;
	//�E�B���h�E�֐��̓o�^
	//DefWindowProc : �f�t�H���g�E�B���h�E�֐�(��Ŏ����ō��)
	wc.lpfnWndProc = WindowProc;
	//�g��Ȃ�(0�Œ�)
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	//�C���X�^���X�ԍ��̓o�^
	wc.hInstance = hInstance;
	//�A�C�R���o�^
	//nullptr : �f�t�H���g
	wc.hIcon = nullptr;
	//�J�[�\���o�^
	//nullptr : �f�t�H���g
	wc.hCursor = nullptr;
	//�E�B���h�E�̔w�i�F
	// LTGRAY_BRUSH : ���邢�D�F
	wc.hbrBackground = (HBRUSH)GetStockObject(1);
	//���j���[�̓o�^
	//nullptr : �f�t�H���g
	wc.lpszMenuName = nullptr;
	//�A�v���P�[�V������
	wc.lpszClassName = "MainWindow";

	//�E�B���h�E��Windows�o�^
	if (RegisterClass(&wc) == 0)
	{
		//�G���[���b�Z�[�W�̕\��
		//MB_OK : OK�{�^���̂�
		//MB_YESNO : OK�E�L�����Z��
		//MB_YESNOCANCEL : �n�C�E�������E�L�����Z��
		MessageBoxA(nullptr,
			"�E�B���h�E�N���X�o�^���s",
			"�G���[���b�Z�[�W",
			MB_YESNOCANCEL);
		return 0;
	}

	//--------------------------------------------
	//�E�B���h�E�̍쐬
	//--------------------------------------------
	hWnd = CreateWindow(
		"MainWindow",		//�A�v���P�[�V������
		"��{�E�B���h�E",		//�E�B���h�E�^�C�g��
		WS_OVERLAPPEDWINDOW,//���ʂ̃E�B���h�E
		100, 100,			//�E�B���h�E�̕\���ʒu(x,y)
		640, 480,			//�E�B���h�E�̕��A����
		nullptr,			//�e�E�B���h�E�̃n���h��
		nullptr,			//���j���[��
		hInstance,			//�C���X�^���X�ԍ�
		nullptr);			//�E�B���h�E�쐬���ɂ͔�������C�x���g�ɓn���f�[�^
	
	if (hWnd == nullptr)
	{
		MessageBox(nullptr,
			"�E�B���h�E�쐬���s",
			"�G���[���b�Z�[�W", MB_OK);
		return 0;
	}
	
	//�E�B���h�E�̕\��
	//SW_SHOW : �\������
	//SW_HIDE :�@�B���i��\���j
	ShowWindow(hWnd, SW_SHOW);



	while (GetMessage(&msg, nullptr, 0, 0) > 0)
	{
		//�E�B���h�E�֐����b�Z�[�W�𑗂�
		DispatchMessage(&msg);
	}

	return 0;
}

