#include "stdafx.h"
#include "JavaClass.h"
#include "JavaMethod.h"
#include "JavaInstructItem.h"

#include <algorithm>


// .method public static saveImageToGallery(Landroid/content/Context;Landroid/graphics/Bitmap;Ljava/lang/String;)V
// ���������ѭ����
bool CJavaMethod::SegmentLoopRecursion(std::vector<int> listSegIndex, int nIndex) {

	if (find(listSegIndex.begin(), listSegIndex.end(), nIndex) != listSegIndex.end()) {
		// �������жϽ�����ѭ��
		// ��������һ����ţ�listSegIndex[listSegIndex.size() - 1];
		// nIndex �����ѭ����ʼ
		CLoopPair loopPair;
		loopPair.nLoopStart = nIndex;
		loopPair.nLoopEnd = listSegIndex[listSegIndex.size() - 1];
		listLooper.push_back(loopPair);

		printf("ѭ���� %d --> %d\n", loopPair.nLoopStart, loopPair.nLoopEnd);

		for (unsigned int i = 0; i < listSegIndex.size(); i++) {
			printf("->%d", listSegIndex[i]);
		}
		printf("\n");


		// �´ο�ʼ����
		while (1) {};
		return true;
	}

	// û�г���ѭ������ӽ�ȥ
	listSegIndex.push_back(nIndex);

	switch (listMethodSegment[nIndex].SegmentNextType) {
	case CJavaMethodSegment::nextNormal:		// ͨ��˳�ӹ�ȥ
	{
		CString strNextLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("next->"));
		int SSI = GetSegmentIndexFromName(strNextLabel);
		if (SSI >= 0) {
			SegmentLoopRecursion(listSegIndex, SSI);
		}
		else {
			printf("1�޷��ҵ������е���ʼλ��\n");
		}
	}
	break;
	case CJavaMethodSegment::nextGoto:			// ͨ����ת��ȥ
	{
		CString strNextLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("goto->"));
		int SSI = GetSegmentIndexFromName(strNextLabel);
		if (SSI >= 0) {
			SegmentLoopRecursion(listSegIndex, SSI);
		}
		else {
			printf("2�޷��ҵ������е���ʼλ��\n");
		}
	}
	break;
	case CJavaMethodSegment::nextIf:			// ͨ��if��ȥ, ��һ����if��ȥ�ģ������� while for ѭ��
	{
		CString strIfyLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("if-y->"));
		CString strIfnLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("if-n->"));

		int SSI = GetSegmentIndexFromName(strIfyLabel);
		if (SSI >= 0) {
			SegmentLoopRecursion(listSegIndex, SSI);
		}
		else {
			printf("3�޷��ҵ������е���ʼλ��\n");
		}
		SSI = GetSegmentIndexFromName(strIfnLabel);
		if (SSI >= 0) {
			SegmentLoopRecursion(listSegIndex, SSI);
		}
		else {
			printf("4�޷��ҵ������е���ʼλ��\n");
		}
	}
	break;
	case CJavaMethodSegment::nextSwitch:		// ͨ��switch�ṹ��ȥ
	{

		CString strSwitchLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("switch->"));
		CString strDefaultLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("default->"));

		// ���� Switch ��֧
		std::vector<CJavaPackedSwitchStruct>::iterator itpss;
		for (itpss = listPackedSwitch.begin(); itpss != listPackedSwitch.end(); itpss++) {
			std::vector<CJavaSwitchItem>::iterator itsit;
			if (strSwitchLabel.Compare((*itpss).strSwitchName) == 0) {
				for (itsit = (*itpss).listItem.begin(); itsit != (*itpss).listItem.end(); itsit++) {
					int SSI = GetSegmentIndexFromName((*itsit).strSegmentName);
					if (SSI >= 0) {
						SegmentLoopRecursion(listSegIndex, SSI);
					}
					else {
						printf("5�޷��ҵ������е���ʼλ��\n");
					}
				}
				break;
			}
		}

		// default ��֧
		int SSI = GetSegmentIndexFromName(strDefaultLabel);
		if (SSI >= 0) {
			SegmentLoopRecursion(listSegIndex, SSI);
		}
		else {
			printf("6�޷��ҵ������е���ʼλ��\n");
		}
	}
	break;
	case CJavaMethodSegment::nextReturn:		// ͨ��return���صĶ�
	{
		return true;
	}
	break;
	case CJavaMethodSegment::nextTryCatch:		// ͨ��TryCatch��֧
	{
		CString strTryLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("try->"));
		CString strDefaultLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("default->"));

		std::vector<CTryCatchNode>::iterator ittcn;
		for (ittcn = listCatchData.begin(); ittcn != listCatchData.end(); ittcn++) {
			std::vector<CCatchItemNode>::iterator itcit;
			if (strTryLabel.Compare((*ittcn).strName) == 0) {


				// try��֧
				int SsiStart = GetSegmentIndexFromName((*ittcn).strStartLabel);
				if (SsiStart >= 0) {
					SegmentLoopRecursion(listSegIndex, SsiStart);
				}
				else {
					printf("181�޷��ҵ������е���ʼλ��\n");
				}

				for (itcit = (*ittcn).listCatch.begin(); itcit != (*ittcn).listCatch.end(); itcit++) {
					int SSI = GetSegmentIndexFromName((*itcit).strCatchLabel);
					if (SSI >= 0) {
						SegmentLoopRecursion(listSegIndex, SSI);
					}
					else {
						printf("7�޷��ҵ������е���ʼλ��\n");
					}
				}
				// final��֧
				if (!(*ittcn).strFinallyLabel.IsEmpty()) {
					int SSI = GetSegmentIndexFromName((*ittcn).strFinallyLabel);
					if (SSI >= 0) {
						SegmentLoopRecursion(listSegIndex, SSI);
					}
					else {
						printf("8�޷��ҵ������е���ʼλ��\n");
					}
				}

				break;
			}
		}

		// default ��֧
		int SSI = GetSegmentIndexFromName(strDefaultLabel);
		if (SSI >= 0) {
			SegmentLoopRecursion(listSegIndex, SSI);
		}
		else {
			printf("9�޷��ҵ������е���ʼλ��\n");
		}
	}
	break;
	case CJavaMethodSegment::nextThrow:			// ���Ҳ�ǽ�������
	{
		return true;
	}
	break;
	default:
	{
		printf("û��ʹ�ù�������\n");
	}
	}

	return true;
}



// �ݹ�ʶ������е�LOOP
void CJavaMethod::CreateSegmentLoopData(void) {
	std::vector<int> listSegIndex;

	int SSI = GetSegmentIndexFromName(CString("main"));

	if (SSI >= 0) {
		SegmentLoopRecursion(listSegIndex, SSI);
	}
	else {
		printf("10�޷��ҵ������е���ʼλ��\n");
	}
}
