#include "stdafx.h"
#include "JavaClass.h"
#include "JavaMethod.h"
#include "JavaInstructItem.h"

#include <algorithm>

// �ݹ飬Ȼ����б�����������
bool CJavaMethod::SegmentVariableSpreadRecursion(std::vector<int> listSegIndex, int nIndex) {

	if (find(listSegIndex.begin(), listSegIndex.end(), nIndex) != listSegIndex.end()) {
		// �������жϽ�����ѭ��
		// ��������һ����ţ�listSegIndex[listSegIndex.size() - 1];
		// nIndex �����ѭ����ʼ
		CLoopPair loopPair;
		loopPair.nLoopStart = nIndex;
		loopPair.nLoopEnd = listSegIndex[listSegIndex.size() - 1];

		printf("ѭ���� %d --> %d\n", loopPair.nLoopStart, loopPair.nLoopEnd);

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
			SegmentVariableSpreadRecursion(listSegIndex, SSI);
		}
		else {
			printf("12�޷��ҵ������е���ʼλ��\n");
		}
	}
	break;
	case CJavaMethodSegment::nextGoto:			// ͨ����ת��ȥ
	{
		CString strNextLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("goto->"));
		int SSI = GetSegmentIndexFromName(strNextLabel);
		if (SSI >= 0) {
			SegmentVariableSpreadRecursion(listSegIndex, SSI);
		}
		else {
			printf("13�޷��ҵ������е���ʼλ��\n");
		}
	}
	break;
	case CJavaMethodSegment::nextIf:			// ͨ��if��ȥ, ��һ����if��ȥ�ģ������� while for ѭ��
	{
		CString strIfyLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("if-y->"));
		CString strIfnLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("if-n->"));

		int SSI = GetSegmentIndexFromName(strIfyLabel);
		if (SSI >= 0) {
			SegmentVariableSpreadRecursion(listSegIndex, SSI);
		}
		else {
			printf("14�޷��ҵ������е���ʼλ��\n");
		}
		SSI = GetSegmentIndexFromName(strIfnLabel);
		if (SSI >= 0) {
			SegmentVariableSpreadRecursion(listSegIndex, SSI);
		}
		else {
			printf("41�޷��ҵ������е���ʼλ��\n");
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
						SegmentVariableSpreadRecursion(listSegIndex, SSI);
					}
					else {
						printf("16�޷��ҵ������е���ʼλ��\n");
					}
				}
				break;
			}
		}

		// default ��֧
		int SSI = GetSegmentIndexFromName(strDefaultLabel);
		if (SSI >= 0) {
			SegmentVariableSpreadRecursion(listSegIndex, SSI);
		}
		else {
			printf("17�޷��ҵ������е���ʼλ��\n");
		}
	}
	break;
	case CJavaMethodSegment::nextReturn:		// ͨ��return���صĶ�
	{
		return SegmentListInstSpread(listSegIndex);
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
					SegmentVariableSpreadRecursion(listSegIndex, SsiStart);
				}
				else {
					printf("181�޷��ҵ������е���ʼλ��\n");
				}

				for (itcit = (*ittcn).listCatch.begin(); itcit != (*ittcn).listCatch.end(); itcit++) {
					int SSI = GetSegmentIndexFromName((*itcit).strCatchLabel);
					if (SSI >= 0) {
						SegmentVariableSpreadRecursion(listSegIndex, SSI);
					}
					else {
						printf("18�޷��ҵ������е���ʼλ��\n");
					}
				}
				// final��֧
				if (!(*ittcn).strFinallyLabel.IsEmpty()) {
					int SSI = GetSegmentIndexFromName((*ittcn).strFinallyLabel);
					if (SSI >= 0) {
						SegmentVariableSpreadRecursion(listSegIndex, SSI);
					}
					else {
						printf("19�޷��ҵ������е���ʼλ��\n");
					}
				}

				break;
			}
		}

		// default ��֧
		int SSI = GetSegmentIndexFromName(strDefaultLabel);
		if (SSI >= 0) {
			SegmentVariableSpreadRecursion(listSegIndex, SSI);
		}
		else {
			printf("20�޷��ҵ������е���ʼλ��\n");
		}
	}
	break;
	case CJavaMethodSegment::nextThrow:			// ���Ҳ�ǽ�������
	{
		// ���õ�β����
		return SegmentListInstSpread(listSegIndex);
	}
	break;
	default:
	{
		printf("û��ʹ�ù�������\n");
	}
	}

	return true;
}


// ͨ���ݹ鷽ʽ���ݳ����мĴ������ܵ�����
void CJavaMethod::SegmentVariableSpread(void) {

	std::vector<int> listSegIndex;

	// ��������
	listSegIndex.clear();
	int SSI = GetSegmentIndexFromName(CString("main"));
	if (SSI >= 0) {
		SegmentVariableSpreadRecursion(listSegIndex, SSI);
	}
	else {
		printf("21�޷��ҵ������е���ʼλ��\n");
	}

	// ����Ƿ����б�����������
	for (unsigned int i = 0; i < this->listOriginalInstruct.size(); i++) {
		if (this->listOriginalInstruct[i].regInfo.listIreg.size() > 0) {	// �����ݼĴ�������ô�����ʸ������Ĵ�������
			for (unsigned int j = 0; j < this->listOriginalInstruct[i].regInfo.listIreg.size(); j++) {
				if (this->listOriginalInstruct[i].regInfo.listIreg[j].strValName.IsEmpty()) {
					printf("�����������û������ %4d %s\n", this->listOriginalInstruct[i].nSerialNumber, this->listOriginalInstruct[i].strInstString);
					while (1) {};
				}
			}
			if (!this->listOriginalInstruct[i].regInfo.outReg.strRegName.IsEmpty()) {

				// ��ʹ�õĲ�������
				if (this->listOriginalInstruct[i].regInfo.outReg.listQuotePosition.size() > 0) {
					if (this->listOriginalInstruct[i].regInfo.outReg.strValName.IsEmpty()) {
						printf("�����������û������ %4d %s\n", this->listOriginalInstruct[i].nSerialNumber, this->listOriginalInstruct[i].strInstString);
						while (1) {};
					}
				}
			}
		}
	}
}