#include "stdafx.h"
#include "JavaClass.h"
#include "JavaMethod.h"
#include "JavaInstructItem.h"

#include <algorithm>

int CJavaMethod::GetSegmentIndexFromName(CString strSegmentName) {
	for (unsigned int i = 0; i < this->listMethodSegment.size(); i++) {
		if (find(this->listMethodSegment[i].listSegName.begin(), this->listMethodSegment[i].listSegName.end(), strSegmentName) != this->listMethodSegment[i].listSegName.end()) {
			return i;
		}
	}
	return -1;
}

CString CJavaMethod::SegmentNextGetOneLabel(std::vector<CString> listStrNext, CString strType) {

	for (unsigned int i = 0; i < listStrNext.size(); i++) {

		if (listStrNext[i].Find(strType) == 0) {
			return listStrNext[i].Right(listStrNext[i].GetLength() - strType.GetLength()).Trim();
		}
	}

	return CString("");
}


bool CJavaMethod::GetThreadFullRunningListRecursion(int nSegIndex, std::vector<std::vector<int>> & listArrayRunningIndex, std::vector<int> listRunningIndex) {


	if (find(listRunningIndex.begin(), listRunningIndex.end(), nSegIndex) != listRunningIndex.end()) {
		// �������жϽ�����ѭ��
		// ��������һ����ţ�listSegIndex[listSegIndex.size() - 1];
		// nIndex �����ѭ����ʼ
		CLoopPair loopPair;
		loopPair.nLoopStart = nSegIndex;
		loopPair.nLoopEnd = listRunningIndex[listRunningIndex.size() - 1];
		listLooper.push_back(loopPair);

		printf("ѭ���� %d --> %d\n", loopPair.nLoopStart, loopPair.nLoopEnd);

		for (unsigned int i = 0; i < listRunningIndex.size(); i++) {
			printf("->%d", listRunningIndex[i]);
		}
		printf("\n");
		// ѭ����ֱ�Ӷ���

		// �´ο�ʼ����
		//while (1) {};
		return true;
	}

#if 1
	// û�г���ѭ������ӽ�ȥ
	listRunningIndex.push_back(nSegIndex);

	switch (listMethodSegment[nSegIndex].SegmentNextType) {
	case CJavaMethodSegment::nextNormal:		// ͨ��˳�ӹ�ȥ
	{
		CString strNextLabel = SegmentNextGetOneLabel(listMethodSegment[nSegIndex].listStrNext, CString("next->"));
		int SSI = GetSegmentIndexFromName(strNextLabel);
		if (SSI >= 0) {
			GetThreadFullRunningListRecursion(SSI, listArrayRunningIndex, listRunningIndex);
		}
		else {
			printf("1�޷��ҵ������е���ʼλ��\n");
		}
	}
	break;
	case CJavaMethodSegment::nextGoto:			// ͨ����ת��ȥ
	{
		CString strNextLabel = SegmentNextGetOneLabel(listMethodSegment[nSegIndex].listStrNext, CString("goto->"));
		int SSI = GetSegmentIndexFromName(strNextLabel);
		if (SSI >= 0) {
			GetThreadFullRunningListRecursion(SSI, listArrayRunningIndex, listRunningIndex);
		}
		else {
			printf("2�޷��ҵ������е���ʼλ��\n");
		}
	}
	break;
	case CJavaMethodSegment::nextIf:			// ͨ��if��ȥ, ��һ����if��ȥ�ģ������� while for ѭ��
	{
		CString strIfyLabel = SegmentNextGetOneLabel(listMethodSegment[nSegIndex].listStrNext, CString("if-y->"));
		CString strIfnLabel = SegmentNextGetOneLabel(listMethodSegment[nSegIndex].listStrNext, CString("if-n->"));

		int SSI = GetSegmentIndexFromName(strIfyLabel);
		if (SSI >= 0) {
			GetThreadFullRunningListRecursion(SSI, listArrayRunningIndex, listRunningIndex);
		}
		else {
			printf("3�޷��ҵ������е���ʼλ��\n");
		}
		SSI = GetSegmentIndexFromName(strIfnLabel);
		if (SSI >= 0) {
			GetThreadFullRunningListRecursion(SSI, listArrayRunningIndex, listRunningIndex);
		}
		else {
			printf("4�޷��ҵ������е���ʼλ��\n");
		}
	}
	break;
	case CJavaMethodSegment::nextSwitch:		// ͨ��switch�ṹ��ȥ
	{

		CString strSwitchLabel = SegmentNextGetOneLabel(listMethodSegment[nSegIndex].listStrNext, CString("switch->"));
		CString strDefaultLabel = SegmentNextGetOneLabel(listMethodSegment[nSegIndex].listStrNext, CString("default->"));

		// ���� Switch ��֧
		std::vector<CJavaPackedSwitchStruct>::iterator itpss;
		for (itpss = listPackedSwitch.begin(); itpss != listPackedSwitch.end(); itpss++) {
			std::vector<CJavaSwitchItem>::iterator itsit;
			if (strSwitchLabel.Compare((*itpss).strSwitchName) == 0) {
				for (itsit = (*itpss).listItem.begin(); itsit != (*itpss).listItem.end(); itsit++) {
					int SSI = GetSegmentIndexFromName((*itsit).strSegmentName);
					if (SSI >= 0) {
						GetThreadFullRunningListRecursion(SSI, listArrayRunningIndex, listRunningIndex);
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
			GetThreadFullRunningListRecursion(SSI, listArrayRunningIndex, listRunningIndex);
		}
		else {
			printf("6�޷��ҵ������е���ʼλ��\n");
		}
	}
	break;
	case CJavaMethodSegment::nextReturn:		// ͨ��return���صĶ�
	{
		listArrayRunningIndex.push_back(listRunningIndex);
		return true;
	}
	break;
	case CJavaMethodSegment::nextTryCatch:		// ͨ��TryCatch��֧
	{
		CString strTryLabel = SegmentNextGetOneLabel(listMethodSegment[nSegIndex].listStrNext, CString("try->"));
		CString strDefaultLabel = SegmentNextGetOneLabel(listMethodSegment[nSegIndex].listStrNext, CString("default->"));

		std::vector<CTryCatchNode>::iterator ittcn;
		for (ittcn = listCatchData.begin(); ittcn != listCatchData.end(); ittcn++) {
			std::vector<CCatchItemNode>::iterator itcit;
			if (strTryLabel.Compare((*ittcn).strName) == 0) {

				// try��֧
				int SsiStart = GetSegmentIndexFromName((*ittcn).strStartLabel);
				if (SsiStart >= 0) {
					GetThreadFullRunningListRecursion(SsiStart, listArrayRunningIndex, listRunningIndex);
				}
				else {
					printf("181�޷��ҵ������е���ʼλ��\n");
				}

				for (itcit = (*ittcn).listCatch.begin(); itcit != (*ittcn).listCatch.end(); itcit++) {
					int SSI = GetSegmentIndexFromName((*itcit).strCatchLabel);
					if (SSI >= 0) {
						GetThreadFullRunningListRecursion(SSI, listArrayRunningIndex, listRunningIndex);
					}
					else {
						printf("7�޷��ҵ������е���ʼλ��\n");
					}
				}
				// final��֧
				if (!(*ittcn).strFinallyLabel.IsEmpty()) {
					int SSI = GetSegmentIndexFromName((*ittcn).strFinallyLabel);
					if (SSI >= 0) {
						GetThreadFullRunningListRecursion(SSI, listArrayRunningIndex, listRunningIndex);
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
			GetThreadFullRunningListRecursion(SSI, listArrayRunningIndex, listRunningIndex);
		}
		else {
			printf("9�޷��ҵ������е���ʼλ��\n");
		}
	}
	break;
	case CJavaMethodSegment::nextThrow:			// ���Ҳ�ǽ�������
	{
		listArrayRunningIndex.push_back(listRunningIndex);
		return true;
	}
	break;
	default:
	{
		printf("û��ʹ�ù�������\n");
	}
	}
#endif

	return true;
}


std::vector<int> CJavaMethod::GetThreadFullRunningList(int nStartSegment, std::vector<int> listIndexSort, int nSegIndex) {

	std::vector<int> listRunningIndex;
	std::vector<std::vector<int>> listArrayRunningIndex;

	// �ݹ���ó���
	listRunningIndex.clear();
	GetThreadFullRunningListRecursion(nSegIndex, listArrayRunningIndex, listRunningIndex);

	for (unsigned int i = 0; i < listArrayRunningIndex.size(); i++) {
		printf("�߳� %4d ��", i);
		for (unsigned int j = 0; j < listArrayRunningIndex[i].size(); j++) {
			printf("%4d ", listArrayRunningIndex[i][j]);
		}
		printf("\n");
	}


	// �����ų�һ���ܱ�
	std::vector<std::vector<int>> listFullIndex;
	for (unsigned int i = 0; i < listArrayRunningIndex.size(); i++) {
		std::vector<int> listRunIndex;
		listRunIndex.clear();
		//printf("����� %d ��", i);
		for (unsigned int j = 0; j < listMethodSegment.size(); j++) {
			if (find(listArrayRunningIndex[i].begin(), listArrayRunningIndex[i].end(), j) != listArrayRunningIndex[i].end()) {
				listRunIndex.push_back(j);
				//printf("%4d,", j);				// ���ھ��б��
			}
			else {
				listRunIndex.push_back(-1);
				//printf("%4d,", -1);				// �����ھ�û�б��
			}
		}
		//printf("\n");
		listFullIndex.push_back(listRunIndex);
	}

	//==================================================>

	// �Ź���ŵ�
	std::vector<std::vector<int>> listFullIndexSort;
	listFullIndexSort.clear();

	if (listIndexSort.size() == listFullIndex[0].size())
	{
		std::vector<int> listOneline;
		listOneline.clear();
		for (unsigned int i = 0; i < listFullIndex.size(); i++) {
			listFullIndexSort.push_back(listOneline);
		}

		for (unsigned int j = 0; j < listIndexSort.size(); j++) {
			for (unsigned int i = 0; i < listFullIndex.size(); i++) {
				int k = (listFullIndex[i])[listIndexSort[j]];
				listFullIndexSort[i].push_back(k);
			}
		}
	}
	else {
		printf("���ش�����������Ĵ�С��һ��");
	}

	std::vector<int> listMerge;
	listMerge.clear();

	// nSegIndex

	int nStartIndex = 0;
	int tmpi = 0;
	for (unsigned int i = 0; i < listIndexSort.size(); i++) {
		if (listIndexSort[i] == nStartSegment) {
			nStartIndex = tmpi;
			break;
		}
	}

	// �۳����Լ���һ��
	nStartIndex++;

	// ��ӡ�����������Խ��
	for (unsigned int i = 0; i < listFullIndexSort.size(); i++) {
		//printf("������ : ");
		if ((listFullIndexSort[i].size() - nStartIndex) > listMerge.size()) {
			listMerge.resize((listFullIndexSort[i].size() - nStartIndex), -1);
		}

		for (unsigned int j = nStartIndex; j < listFullIndexSort[i].size(); j++) {

			if ((listFullIndexSort[i])[j] >= 0) {
				listMerge[j - nStartIndex] = (listFullIndexSort[i])[j];
			}

			//printf("%4d ", (listFullIndexSort[i])[j]);
		}
		//printf("\n");
	}

	printf("��һ�������� ��");
	for (unsigned int i = 0; i < listMerge.size(); i++) {
		printf("%4d ", listMerge[i]);
	}
	printf("\n");

	return listMerge;
}

int CMarcoCodeItem::nSeed = 10000;

// ���غ�ָ������
int CJavaMethod::MacroStructureRecursion(int nStartSeg, std::vector<int> listIndexSort, int nEndSeg) {

	CMarcoCodeItem  macroStruct;
	macroStruct.clear();

	// ��ǰ�����ĺ�ṹ, ����Ϊ�ṹ��꺯��
	CString strMacroInst;

	int SSI = nStartSeg;


	// ѭ���߽�
	while ((SSI >= 0) && (SSI != nEndSeg)) {

		switch (listMethodSegment[SSI].SegmentNextType) {
		case CJavaMethodSegment::nextNormal:		// ͨ��˳�ӹ�ȥ
		{
			CString strNextLabel = SegmentNextGetOneLabel(listMethodSegment[SSI].listStrNext, CString("next->"));
			strMacroInst.Format("normal %d", SSI);
			macroStruct.listMacro.push_back(strMacroInst);
			SSI = GetSegmentIndexFromName(strNextLabel);
		}
		break;
		case CJavaMethodSegment::nextGoto:			// ͨ����ת��ȥ
		{
			CString strNextLabel = SegmentNextGetOneLabel(listMethodSegment[SSI].listStrNext, CString("goto->"));
			strMacroInst.Format("normal %d", SSI);
			macroStruct.listMacro.push_back(strMacroInst);
			SSI = GetSegmentIndexFromName(strNextLabel);
		}
		break;
		case CJavaMethodSegment::nextIf:			// ͨ��if��ȥ, ��һ����if��ȥ�ģ������� while for ѭ��
		{
			CString strIfyLabel = SegmentNextGetOneLabel(listMethodSegment[SSI].listStrNext, CString("if-y->"));
			CString strIfnLabel = SegmentNextGetOneLabel(listMethodSegment[SSI].listStrNext, CString("if-n->"));

			// 1�����������һ�����б�
			// 2��Ȼ���ҵ�if�ıջ�λ��
			// 3�����ݱջ�λ�ã�ͳ�����߲�һ�µ�λ��
			int SsiIfyIndex = GetSegmentIndexFromName(strIfyLabel);
			int SsiIfnIndex = GetSegmentIndexFromName(strIfnLabel);
			std::vector<int> listRunningIfy = GetThreadFullRunningList(SSI, listIndexSort, SsiIfyIndex);
			std::vector<int> listRunningIfn = GetThreadFullRunningList(SSI, listIndexSort, SsiIfnIndex);

			if (listRunningIfy.size() != listRunningIfn.size()) {
				printf("���ش���������֧ͳ�Ƴ����Ĵ�С��һ��\n");
				while (1) {};
			}

			unsigned int nOffset = 0;
			unsigned int nEndSegmentIndex = 1000000;

			for (nOffset = 0; nOffset < listRunningIfy.size(); nOffset++) {
				if (listRunningIfy[nOffset] >= 0) {
					if (listRunningIfy[nOffset] == listRunningIfn[nOffset]) {
						nEndSegmentIndex = listRunningIfy[nOffset];
						break;
					}
				}
			}

			printf("if �ṹ�Ľ���λ�� %d\n", nEndSegmentIndex);

			int nIfyBlockCount = 0;
			int nIfnBlockCount = 0;
			for (unsigned int i = 0; i < nOffset; i++) {
				if (listRunningIfy[i] >= 0) {
					nIfyBlockCount++;
				}
				if (listRunningIfn[i] >= 0) {
					nIfnBlockCount++;
				}
			}

			int nMacroIfy = 0;
			int nMacroIfn = 0;

			if (nIfyBlockCount > 0) {
				// SsiIfyIndex -> listRunningIfy[nOffset] ����ôһ�Σ� listRunningIfy[nOffset] �����ʡ���ܰ�������
				nMacroIfy = MacroStructureRecursion(SsiIfyIndex, listIndexSort, nEndSegmentIndex);
			}
			if (nIfnBlockCount > 0) {
				nMacroIfn = MacroStructureRecursion(SsiIfnIndex, listIndexSort, nEndSegmentIndex);
			}

			if ((nIfyBlockCount <= 0) && (nIfnBlockCount <= 0)) {
				// �յ� if ���
				strMacroInst.Format("if %d,%d,%d", SSI, nMacroIfy, nMacroIfn);
				macroStruct.listMacro.push_back(strMacroInst);
			}
			else if ((nIfyBlockCount > 0) && (nIfnBlockCount <= 0)) {
				// yes �����У� no ����û�У���Ҫ��ת����

				strMacroInst.Format("if %d,%d,%d", SSI, nMacroIfy, nMacroIfn);
				macroStruct.listMacro.push_back(strMacroInst);
			}
			else if ((nIfyBlockCount <= 0) && (nIfnBlockCount > 0)) {
				// no �����У� yes ����û��
				strMacroInst.Format("if %d,%d,%d", SSI, nMacroIfy, nMacroIfn);
				macroStruct.listMacro.push_back(strMacroInst);
			}
			else {
				// if Ҳ�� else Ҳ��
				strMacroInst.Format("if %d,%d,%d", SSI, nMacroIfy, nMacroIfn);
				macroStruct.listMacro.push_back(strMacroInst);
			}

			// ��һ��SSS �Ĵ���
			SSI = nEndSegmentIndex;
		}
		break;
		case CJavaMethodSegment::nextSwitch:		// ͨ��switch�ṹ��ȥ
		{
			CString strSwitchLabel = SegmentNextGetOneLabel(listMethodSegment[SSI].listStrNext, CString("switch->"));
			CString strDefaultLabel = SegmentNextGetOneLabel(listMethodSegment[SSI].listStrNext, CString("default->"));

			int SsiDefaultIndex = GetSegmentIndexFromName(strDefaultLabel);
			int nOutPutNextSegment = SsiDefaultIndex;

			for (unsigned int i = 0; i < this->listPackedSwitch.size(); i++) {

				if (this->listPackedSwitch[i].strSwitchName == strSwitchLabel) {

					std::vector<std::vector<int>> listArrayRunningFullCase;
					listArrayRunningFullCase.clear();

					for (unsigned int j = 0; j < listPackedSwitch[i].listItem.size(); j++) {
						int SsiCaseIndex = GetSegmentIndexFromName(listPackedSwitch[i].listItem[j].strSegmentName);
						std::vector<int> listRunning = GetThreadFullRunningList(SSI, listIndexSort, SsiCaseIndex);
						listArrayRunningFullCase.push_back(listRunning);
					}

					// �� strDefaultLabel ����
					int SsiStartIndex = GetSegmentIndexFromName(strDefaultLabel);
					std::vector<int> listRunning = GetThreadFullRunningList(SSI, listIndexSort, SsiStartIndex);
					listArrayRunningFullCase.push_back(listRunning);


					// Ѱ��ÿ�������Ƶ�λ��
					unsigned int nOffset = 0;
					unsigned int nEndSegmentIndex = 1000000;
					for (nOffset = 0; nOffset < listArrayRunningFullCase[0].size(); nOffset++) {
						if (listArrayRunningFullCase[0][nOffset] >= 0) {

							int nData = listArrayRunningFullCase[0][nOffset];
							bool bAllQq = true;
							for (unsigned int s = 0; s < listArrayRunningFullCase.size(); s++) {
								if (nData != listArrayRunningFullCase[s][nOffset]) {
									bAllQq = false;
									break;
								}
							}
							nEndSegmentIndex = nData;
							if (bAllQq) {
								break;
							}
						}
					}

					std::vector<int> listCatchMacro;
					listCatchMacro.clear();
					for (unsigned int j = 0; j < listPackedSwitch[i].listItem.size(); j++) {
						int SsiCatchIndex = GetSegmentIndexFromName(listPackedSwitch[i].listItem[j].strSegmentName);
						int nCatchMacroIndex = MacroStructureRecursion(SsiCatchIndex, listIndexSort, nEndSegmentIndex);
						listCatchMacro.push_back(nCatchMacroIndex);
					}

					int nMacroDefault = 0;

					if (SsiDefaultIndex != nEndSegmentIndex) {
						nMacroDefault = MacroStructureRecursion(SsiDefaultIndex, listIndexSort, nEndSegmentIndex);
					}

					if (nEndSegmentIndex >= 1000000) {
						nOutPutNextSegment = SsiDefaultIndex;
					}
					else {
						nOutPutNextSegment = nEndSegmentIndex;
					}

					strMacroInst.Format("switch %d,%d", SSI, nMacroDefault);
					for (unsigned int j = 0; j < listPackedSwitch[i].listItem.size(); j++) {
						CString strEntry;
						strEntry.Format(",%d,%d", listPackedSwitch[i].listItem[j].nData, listCatchMacro[j]);
						strMacroInst += strEntry;
					}

					macroStruct.listMacro.push_back(strMacroInst);
					break;
				}
			}

			SSI = nOutPutNextSegment;

		}
		break;
		case CJavaMethodSegment::nextReturn:		// ͨ��return���صĶ�
		{
			// ������ǵ�ǰһ�Σ� listSegIndex ���ǵ�ǰ���

			// �����ָ��
			strMacroInst.Format("return %d", SSI);
			macroStruct.listMacro.push_back(strMacroInst);
			SSI = -1;

		}
		break;
		case CJavaMethodSegment::nextTryCatch:		// ͨ��TryCatch��֧
		{
			CString strTryLabel = SegmentNextGetOneLabel(listMethodSegment[SSI].listStrNext, CString("try->"));
			CString strDefaultLabel = SegmentNextGetOneLabel(listMethodSegment[SSI].listStrNext, CString("default->"));


			int SsiDefaultIndex = GetSegmentIndexFromName(strDefaultLabel);

			for (unsigned int i = 0; i < this->listCatchData.size(); i++) {

				if (this->listCatchData[i].strName == strTryLabel) {

					// ���������
					CString strName = listCatchData[i].strName;
					CString strStartLabel = listCatchData[i].strStartLabel;
					CString strEndLabel = listCatchData[i].strEndLabel;
					std::vector<CCatchItemNode> listCatch = listCatchData[i].listCatch;
					CString strFinallyLabel = listCatchData[i].strFinallyLabel;

					std::vector<std::vector<int>> listArrayRunningFullCatch;
					listArrayRunningFullCatch.clear();

					// �� catch ����
					for (unsigned int j = 0; j < listCatch.size(); j++) {
						int SsiCatchIndex = GetSegmentIndexFromName(listCatch[j].strCatchLabel);
						std::vector<int> listRunning = GetThreadFullRunningList(SSI, listIndexSort, SsiCatchIndex);
						listArrayRunningFullCatch.push_back(listRunning);
					}
					// �� strStartLabel ����
					int SsiStartIndex = GetSegmentIndexFromName(strStartLabel);
					std::vector<int> listRunning = GetThreadFullRunningList(SSI, listIndexSort, SsiStartIndex);
					listArrayRunningFullCatch.push_back(listRunning);

					// Ѱ��ÿ�������Ƶ�λ��
					unsigned int nOffset = 0;
					unsigned int nEndSegmentIndex = 1000000;
					for (nOffset = 0; nOffset < listArrayRunningFullCatch[0].size(); nOffset++) {
						if (listArrayRunningFullCatch[0][nOffset] >= 0) {

							int nData = listArrayRunningFullCatch[0][nOffset];
							bool bAllQq = true;
							for (unsigned int s = 0; s < listArrayRunningFullCatch.size(); s++) {
								if (nData != listArrayRunningFullCatch[s][nOffset]) {
									bAllQq = false;
									break;
								}
							}
							nEndSegmentIndex = nData;
							if (bAllQq) {
								break;
							}
						}
					}

					if (nEndSegmentIndex != SsiDefaultIndex) {
						// Catch ���˳�λ�ò�һ�µ����
						printf("Catch ���˳�λ�ò�һ�µ����\n");
						while (1) {};
					}
					std::vector<int> listCatchMacro;
					listCatchMacro.clear();
					for (unsigned int j = 0; j < listCatch.size(); j++) {
						int SsiCatchIndex = GetSegmentIndexFromName(listCatch[j].strCatchLabel);
						int nCatchMacroIndex = MacroStructureRecursion(SsiCatchIndex, listIndexSort, nEndSegmentIndex);
						listCatchMacro.push_back(nCatchMacroIndex);
					}
					int nMacroFinaly = 0;
					if (!strFinallyLabel.IsEmpty()) {
						int SsiFinalyIndex = GetSegmentIndexFromName(strFinallyLabel);
						nMacroFinaly = MacroStructureRecursion(SsiFinalyIndex, listIndexSort, nEndSegmentIndex);
					}
					int nMacroTry = 0;
					if ((!strStartLabel.IsEmpty()) && (!strEndLabel.IsEmpty())) {
						int SsiStartIndex = GetSegmentIndexFromName(strStartLabel);
						int SsiEndIndex = GetSegmentIndexFromName(strEndLabel);
						nMacroTry = MacroStructureRecursion(SsiStartIndex, listIndexSort, SsiEndIndex);
					}

					strMacroInst.Format("try %d,%d,%d", SSI, nMacroTry, nMacroFinaly);
					for (unsigned int j = 0; j < listCatch.size(); j++) {
						CString strEntry;
						strEntry.Format(",%s,%d", listCatch[j].strCatchType, listCatchMacro[j]);
						strMacroInst += strEntry;
					}

					macroStruct.listMacro.push_back(strMacroInst);
					break;
				}
			}


			SSI = GetSegmentIndexFromName(strDefaultLabel);
		}
		break;
		case CJavaMethodSegment::nextThrow:			// ���Ҳ�ǽ�������
		{
			// �����ָ��
			strMacroInst.Format("throw %d", SSI);
			macroStruct.listMacro.push_back(strMacroInst);
			SSI = -1;
		}
		break;
		default:
		{
			printf("û��ʹ�ù�������\n");
		}
		}

		// ��ֹ�����ж�
	}


	// ������з���
	this->listMarco.push_back(macroStruct);
	return macroStruct.nMacroIndex;
}

void CJavaMethod::ShowAbsMacroStructRecursion(std::vector<int> listOldInstList, int nLevel, int nMacroIndex) {

	for (unsigned int i = 0; i < listMarco.size(); i++) {
		if (listMarco[i].nMacroIndex == nMacroIndex) {

			for (unsigned int j = 0; j < listMarco[i].listMacro.size(); j++) {
				CString strMacro = listMarco[i].listMacro[j];

				std::vector<CString> listI = CJavaInstructItem::GetListInstructionSymbol(strMacro);

				for (int k = 0; k < nLevel; k++) { printf("\t"); }
				printf("%s\n", strMacro);

				if (listI[0] == "return") {
					//------------------------------------------------------->
				}
				else if (listI[0] == "throw") {
				}
				else if (listI[0] == "normal") {
				}
				else if (listI[0] == "if") {

					for (int k = 0; k < nLevel; k++) { printf("\t"); }
					printf("if ( %s ) {\n", listI[1]);

					if (atoi(listI[2]) > 0) {
						ShowAbsMacroStructRecursion(listOldInstList, nLevel + 1, atoi(listI[2]));
					}
					if (atoi(listI[3]) > 0) {
						for (int k = 0; k < nLevel; k++) { printf("\t"); }
						printf("} else {\n", listI[1]);
						ShowAbsMacroStructRecursion(listOldInstList, nLevel + 1, atoi(listI[3]));
					}
					for (int k = 0; k < nLevel; k++) { printf("\t"); }
					printf("}\n");
				}
				else if (listI[0] == "try") {

					CString strFinaly = listI[3];

					for (int k = 0; k < nLevel; k++) { printf("\t"); }
					printf("try (%s) {\n", listI[1]);

					ShowAbsMacroStructRecursion(listOldInstList, nLevel + 1, atoi(listI[2]));

					for (int k = 0; k < nLevel; k++) { printf("\t"); }
					printf("}\n");

					for (unsigned int s = 0; s < (listI.size() - 4) / 2; s++) {

						CString strType = listI[4 + s * 2 + 0];
						CString strLabel = listI[4 + s * 2 + 1];

						for (int k = 0; k < nLevel; k++) { printf("\t"); }
						printf("catch (%s) : %s {\n", strType, strLabel);

						ShowAbsMacroStructRecursion(listOldInstList, nLevel + 1, atoi(strLabel));

						for (int k = 0; k < nLevel; k++) { printf("\t"); }
						printf("}\n");
					}
					if (atoi(strFinaly) > 0) {

						for (int k = 0; k < nLevel; k++) { printf("\t"); }
						printf("finaly (%s) {\n", strFinaly);

						ShowAbsMacroStructRecursion(listOldInstList, nLevel + 1, atoi(strFinaly));

						for (int k = 0; k < nLevel; k++) { printf("\t"); }
						printf("}\n");
					}
				}
				else if (listI[0] == "switch") {
					CString strDefault = listI[2];

					for (int k = 0; k < nLevel; k++) { printf("\t"); }
					printf("switch (%s) {\n", listI[1]);


					for (unsigned int s = 0; s < (listI.size() - 3) / 2; s++) {
						CString strValue = listI[3 + s * 2 + 0];
						CString strLabel = listI[3 + s * 2 + 1];

						for (int k = 0; k < nLevel; k++) { printf("\t"); }
						printf("case (%s) : %s {\n", strValue, strLabel);

						ShowAbsMacroStructRecursion(listOldInstList, nLevel + 1, atoi(strLabel));

						for (int k = 0; k < nLevel; k++) { printf("\t"); }
						printf("}\n");
						for (int k = 0; k < nLevel; k++) { printf("\t"); }
						printf("break;\n");
					}

					if (atoi(strDefault) > 0) {
						for (int k = 0; k < nLevel; k++) { printf("\t"); }
						printf("default : %s {\n", strDefault);

						ShowAbsMacroStructRecursion(listOldInstList, nLevel + 1, atoi(strDefault));

						for (int k = 0; k < nLevel; k++) { printf("\t"); }
						printf("}\n");
						for (int k = 0; k < nLevel; k++) { printf("\t"); }
						printf("break;\n");
					}

					for (int k = 0; k < nLevel; k++) { printf("\t"); }
					printf("}\n");
				}
				else {
					printf("01����ʶ��ĺ�ָ�� %s\n", strMacro);
					while (1) {};
				}

			}

			return;
		}
	}

	printf("�Ҳ����ĺ�ṹ");

}

CString CJavaMethod::GetCatchValueName(int nMacroIndex) {
	for (unsigned int i = 0; i < listMarco.size(); i++) {
		if (listMarco[i].nMacroIndex == nMacroIndex) {

			CString strMacro = listMarco[i].listMacro[0];
			std::vector<CString> listI = CJavaInstructItem::GetListInstructionSymbol(strMacro);
			int nSegmentIndex = atoi(listI[1]);
			int nInstIndex = listMethodSegment[nSegmentIndex].listInstSn[0];

			std::vector<CJavaInstructItem>::iterator it = this->GetOriginalInstruct(nInstIndex);
			if (it == listOriginalInstruct.end()) {
				printf("�Ҳ������ %4d ��Ӧ��ָ��\n", nInstIndex);
				while (1) {};
			}

			if (it->strInstString.Find("catch") != 0) {
				printf("���ָ��� catch ָ��\n");
				while (1) {};
			}

			return it->regInfo.outReg.strValName;
		}
	}

	return CString("Unknow!");
}

// �ݹ���ʾ��ṹ
void CJavaMethod::TranslateMacroStructRecursion(std::vector<int> listOldInstList, int nLevel, int nMacroIndex) {


	//std::vector<int> listOldInstIndex;

	for (unsigned int i = 0; i < listMarco.size(); i++) {
		if (listMarco[i].nMacroIndex == nMacroIndex) {

			for (unsigned int j = 0; j < listMarco[i].listMacro.size(); j++) {
				CString strMacro = listMarco[i].listMacro[j];

				std::vector<CString> listI = CJavaInstructItem::GetListInstructionSymbol(strMacro);


				if (listI[0] == "return") {
					//------------------------------------------------------->
					int nSegmentIndex = atoi(listI[1]);
					std::vector<int> listCurrentInstIndex;
					listCurrentInstIndex.clear();
					for (unsigned int k = 0; k < listMethodSegment[nSegmentIndex].listInstSn.size(); k++) {
						listCurrentInstIndex.push_back(listMethodSegment[nSegmentIndex].listInstSn[k]);
					}

					// ����ָ��
					SegmentCodeTranslate(nLevel, listOldInstList, listCurrentInstIndex);
					listOldInstList.insert(listOldInstList.end(), listCurrentInstIndex.begin(), listCurrentInstIndex.end());
				}
				else if (listI[0] == "throw") {
					int nSegmentIndex = atoi(listI[1]);
					std::vector<int> listCurrentInstIndex;
					listCurrentInstIndex.clear();
					for (unsigned int k = 0; k < listMethodSegment[nSegmentIndex].listInstSn.size(); k++) {
						listCurrentInstIndex.push_back(listMethodSegment[nSegmentIndex].listInstSn[k]);
					}
					// ����ָ��
					SegmentCodeTranslate(nLevel, listOldInstList, listCurrentInstIndex);
					listOldInstList.insert(listOldInstList.end(), listCurrentInstIndex.begin(), listCurrentInstIndex.end());
				}
				else if (listI[0] == "normal") {
					int nSegmentIndex = atoi(listI[1]);
					std::vector<int> listCurrentInstIndex;
					listCurrentInstIndex.clear();
					for (unsigned int k = 0; k < listMethodSegment[nSegmentIndex].listInstSn.size(); k++) {
						listCurrentInstIndex.push_back(listMethodSegment[nSegmentIndex].listInstSn[k]);
					}
					// ����ָ��
					SegmentCodeTranslate(nLevel, listOldInstList, listCurrentInstIndex);
					listOldInstList.insert(listOldInstList.end(), listCurrentInstIndex.begin(), listCurrentInstIndex.end());
				}
				else if (listI[0] == "if") {

					int nSegmentIndex = atoi(listI[1]);
					std::vector<int> listCurrentInstIndex;
					listCurrentInstIndex.clear();
					for (unsigned int k = 0; k < listMethodSegment[nSegmentIndex].listInstSn.size(); k++) {
						listCurrentInstIndex.push_back(listMethodSegment[nSegmentIndex].listInstSn[k]);
					}
					// ����ָ��
					SegmentCodeTranslate(nLevel, listOldInstList, listCurrentInstIndex);
					listOldInstList.insert(listOldInstList.end(), listCurrentInstIndex.begin(), listCurrentInstIndex.end());

					int nLastInstIndex = listCurrentInstIndex[listCurrentInstIndex.size() - 1];

					std::vector<CJavaInstructItem>::iterator itLastInst = GetOriginalInstruct(nLastInstIndex);
					if (itLastInst == this->listOriginalInstruct.end()) {
						printf("�Ҳ�����ָ��\n");
						return;
					}

					int nIfyIndex = atoi(listI[2]);
					int nIfnIndex = atoi(listI[3]);

					if ((nIfyIndex > 0) && (nIfnIndex > 0)) {
						printf("%s {\n", itLastInst->strCppExpression);
						this->InsertCppStringCode(nLevel + 1, itLastInst->strCppExpression + CString(" {"));

						TranslateMacroStructRecursion(listOldInstList, nLevel + 1, nIfyIndex);
						printf("}else{\n");

						this->InsertCppStringCode(nLevel + 1, CString("} else {"));

						TranslateMacroStructRecursion(listOldInstList, nLevel + 1, nIfnIndex);
						printf("}\n");
						this->InsertCppStringCode(nLevel + 1, CString("}"));
					}
					else if (nIfyIndex > 0) {
						printf("%s {\n", itLastInst->strCppExpression);
						this->InsertCppStringCode(nLevel + 1, itLastInst->strCppExpression + CString(" {"));
						TranslateMacroStructRecursion(listOldInstList, nLevel + 1, nIfyIndex);
						printf("}\n");
						this->InsertCppStringCode(nLevel + 1, CString("}"));
					}
					else if (nIfnIndex > 0) {
						printf("%s {\n", itLastInst->strCppInverseExpression);
						this->InsertCppStringCode(nLevel + 1, itLastInst->strCppInverseExpression + CString(" {"));
						TranslateMacroStructRecursion(listOldInstList, nLevel + 1, nIfnIndex);
						printf("}\n");
						this->InsertCppStringCode(nLevel + 1, CString("}"));
					}
					else {
						printf("%s {\n", itLastInst->strCppExpression);
						this->InsertCppStringCode(nLevel + 1, itLastInst->strCppExpression + CString(" {"));
						printf("}\n");
						this->InsertCppStringCode(nLevel + 1, CString("}"));
					}
				}
				else if (listI[0] == "try") {

					int nSegmentIndex = atoi(listI[1]);
					std::vector<int> listCurrentInstIndex;
					listCurrentInstIndex.clear();
					for (unsigned int k = 0; k < listMethodSegment[nSegmentIndex].listInstSn.size(); k++) {
						listCurrentInstIndex.push_back(listMethodSegment[nSegmentIndex].listInstSn[k]);
					}
					// ����ָ��
					SegmentCodeTranslate(nLevel, listOldInstList, listCurrentInstIndex);
					listOldInstList.insert(listOldInstList.end(), listCurrentInstIndex.begin(), listCurrentInstIndex.end());

					int nLastInstIndex = listCurrentInstIndex[listCurrentInstIndex.size() - 1];

					std::vector<CJavaInstructItem>::iterator itLastInst = GetOriginalInstruct(nLastInstIndex);
					if (itLastInst == this->listOriginalInstruct.end()) {
						printf("�Ҳ�����ָ��\n");
						return;
					}


					CString strFinaly = listI[3];
					for (int k = 0; k < nLevel; k++) { printf("\t"); }
					printf("try{\n");
					TranslateMacroStructRecursion(listOldInstList, nLevel + 1, atoi(listI[2]));
					for (int k = 0; k < nLevel; k++) { printf("\t"); }
					printf("}\n");
					for (unsigned int s = 0; s < (listI.size() - 4) / 2; s++) {
						CString strType = listI[4 + s * 2 + 0];
						CString strLabel = listI[4 + s * 2 + 1];
						// ����Ҫ��� catch �ı���
						CString strCatchName = GetCatchValueName(atoi(strLabel));
						for (int k = 0; k < nLevel; k++) { printf("\t"); }
						printf("catch (%s %s){\n", strType, strCatchName);
						TranslateMacroStructRecursion(listOldInstList, nLevel + 1, atoi(strLabel));
						for (int k = 0; k < nLevel; k++) { printf("\t"); }
						printf("}\n");
					}
					if (atoi(strFinaly) > 0) {
						for (int k = 0; k < nLevel; k++) { printf("\t"); }
						printf("finaly{\n", strFinaly);
						TranslateMacroStructRecursion(listOldInstList, nLevel + 1, atoi(strFinaly));
						for (int k = 0; k < nLevel; k++) { printf("\t"); }
						printf("}\n");
					}
				}
				else if (listI[0] == "switch") {

					int nSegmentIndex = atoi(listI[1]);
					std::vector<int> listCurrentInstIndex;
					listCurrentInstIndex.clear();
					for (unsigned int k = 0; k < listMethodSegment[nSegmentIndex].listInstSn.size(); k++) {
						listCurrentInstIndex.push_back(listMethodSegment[nSegmentIndex].listInstSn[k]);
					}
					// ����ָ��
					SegmentCodeTranslate(nLevel, listOldInstList, listCurrentInstIndex);
					listOldInstList.insert(listOldInstList.end(), listCurrentInstIndex.begin(), listCurrentInstIndex.end());

					int nLastInstIndex = listCurrentInstIndex[listCurrentInstIndex.size() - 1];

					std::vector<CJavaInstructItem>::iterator itLastInst = GetOriginalInstruct(nLastInstIndex);
					if (itLastInst == this->listOriginalInstruct.end()) {
						printf("�Ҳ�����ָ��\n");
						return;
					}

					CString strDefault = listI[2];

					for (int k = 0; k < nLevel; k++) { printf("\t"); }
					printf("(%s) {\n", itLastInst->strCppExpression);


					for (unsigned int s = 0; s < (listI.size() - 3) / 2; s++) {
						CString strValue = listI[3 + s * 2 + 0];
						CString strLabel = listI[3 + s * 2 + 1];

						for (int k = 0; k < nLevel; k++) { printf("\t"); }
						printf("case (%s) : {\n", strValue);

						TranslateMacroStructRecursion(listOldInstList, nLevel + 1, atoi(strLabel));

						for (int k = 0; k < nLevel; k++) { printf("\t"); }
						printf("}\n");
						for (int k = 0; k < nLevel; k++) { printf("\t"); }
						printf("break;\n");
					}

					if (atoi(strDefault) > 0) {
						for (int k = 0; k < nLevel; k++) { printf("\t"); }
						printf("default : {\n");

						TranslateMacroStructRecursion(listOldInstList, nLevel + 1, atoi(strDefault));

						for (int k = 0; k < nLevel; k++) { printf("\t"); }
						printf("}\n");
						for (int k = 0; k < nLevel; k++) { printf("\t"); }
						printf("break;\n");
					}

					for (int k = 0; k < nLevel; k++) { printf("\t"); }
					printf("}\n");
				}
				else {
					printf("02����ʶ��ĺ�ָ�� %s\n", strMacro);
					while (1) {};
				}

			}

			return;
		}
	}

	printf("�Ҳ����ĺ�ṹ");

}


// .method public static saveImageToGallery(Landroid/content/Context;Landroid/graphics/Bitmap;Ljava/lang/String;)V
// �ݹ�ķ�ʽ���ҳ����п��ܵ�����·����������listArrayRunningIndex��
//                                            ���ڱ�����ʱ·���Ķ���        ȫ�ֱ���                                              ��ʼ�Ķ�����
bool CJavaMethod::GetFullRunVectorRecursion(std::vector<int> listSegIndex, std::vector<std::vector<int>> & listArrayRunningIndex, int nIndex) {

	if (find(listSegIndex.begin(), listSegIndex.end(), nIndex) != listSegIndex.end()) {
		listArrayRunningIndex.push_back(listSegIndex);
		return true;
	}

	// ��ǰ����ӽ�ȥ
	listSegIndex.push_back(nIndex);

	switch (listMethodSegment[nIndex].SegmentNextType) {
	case CJavaMethodSegment::nextNormal:		// ͨ��˳�ӹ�ȥ
	{
		CString strNextLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("next->"));
		int SSI = GetSegmentIndexFromName(strNextLabel);
		if (SSI >= 0) {
			GetFullRunVectorRecursion(listSegIndex, listArrayRunningIndex, SSI);
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
			GetFullRunVectorRecursion(listSegIndex, listArrayRunningIndex, SSI);
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
			GetFullRunVectorRecursion(listSegIndex, listArrayRunningIndex, SSI);
		}
		else {
			printf("3�޷��ҵ������е���ʼλ��\n");
		}
		SSI = GetSegmentIndexFromName(strIfnLabel);
		if (SSI >= 0) {
			GetFullRunVectorRecursion(listSegIndex, listArrayRunningIndex, SSI);
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
						GetFullRunVectorRecursion(listSegIndex, listArrayRunningIndex, SSI);
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
			GetFullRunVectorRecursion(listSegIndex, listArrayRunningIndex, SSI);
		}
		else {
			printf("6�޷��ҵ������е���ʼλ��\n");
		}
	}
	break;
	case CJavaMethodSegment::nextReturn:		// ͨ��return���صĶ�
	{
		listArrayRunningIndex.push_back(listSegIndex);
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

				// try ��֧
				int SsiStart = GetSegmentIndexFromName((*ittcn).strStartLabel);
				if (SsiStart >= 0) {
					GetFullRunVectorRecursion(listSegIndex, listArrayRunningIndex, SsiStart);
				}
				else {
					printf("181�޷��ҵ������е���ʼλ��\n");
				}

				// catch ��֧
				for (itcit = (*ittcn).listCatch.begin(); itcit != (*ittcn).listCatch.end(); itcit++) {
					int SSI = GetSegmentIndexFromName((*itcit).strCatchLabel);
					if (SSI >= 0) {
						GetFullRunVectorRecursion(listSegIndex, listArrayRunningIndex, SSI);
					}
					else {
						printf("7�޷��ҵ������е���ʼλ��\n");
					}
				}
				// final ��֧
				if (!(*ittcn).strFinallyLabel.IsEmpty()) {
					int SSI = GetSegmentIndexFromName((*ittcn).strFinallyLabel);
					if (SSI >= 0) {
						GetFullRunVectorRecursion(listSegIndex, listArrayRunningIndex, SSI);
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
			GetFullRunVectorRecursion(listSegIndex, listArrayRunningIndex, SSI);
		}
		else {
			printf("9�޷��ҵ������е���ʼλ��\n");
		}
	}
	break;
	case CJavaMethodSegment::nextThrow:			// ���Ҳ�ǽ�������
	{
		listArrayRunningIndex.push_back(listSegIndex);
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


// ���Ӿ�ϸ�ļ��������̣߳����֮ǰ����������
int CJavaMethod::CreateMacroCodeStruction(void) {
	std::vector<int> listSegIndex;
	listSegIndex.clear();

	CMarcoCodeItem::ResetSeed();
	listMarco.clear();

	// ����һ�ο�ʼ���нṹ���и�
	//listSegIndex.clear();
	int SSI = GetSegmentIndexFromName(CString("main"));

	if (SSI >= 0) {

		int nMacroIndex = MacroStructureRecursion(SSI, listFullRunVector, 1000000);

		printf("��ʾ��ṹ\n");
		ShowAbsMacroStructRecursion(listSegIndex, 0, nMacroIndex);

		return nMacroIndex;


	}

	return -1;
}

// ��ʼ�����ṹ
void CJavaMethod::TranslateMacroCodeStruct(int nMacroIndex) {
	std::vector<int> listSegIndex;
	listSegIndex.clear();

	// ���������ʼ����
	printf("��ʾ����\n");
	if (nMacroIndex > 0) {
		TranslateMacroStructRecursion(listSegIndex, 0, nMacroIndex);
	}
}



