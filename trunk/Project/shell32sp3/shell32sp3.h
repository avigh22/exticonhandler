// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
//��ı�׼�������� DLL �е������ļ��������������϶���� SHELL32SP3_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
//�κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ 
// SHELL32SP3_API ������Ϊ�ǴӴ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef SHELL32SP3_EXPORTS
#define SHELL32SP3_API __declspec(dllexport)
#else
#define SHELL32SP3_API __declspec(dllimport)
#endif

// �����Ǵ� shell32sp3.dll ������
class SHELL32SP3_API Cshell32sp3 {
public:
	Cshell32sp3(void);
	// TODO: �ڴ�������ķ�����
};

extern SHELL32SP3_API int nshell32sp3;

SHELL32SP3_API int fnshell32sp3(void);
