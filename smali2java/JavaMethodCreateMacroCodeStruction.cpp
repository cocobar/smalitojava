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
		// 这里是判断进入了循环
		// 这个是最后一个序号：listSegIndex[listSegIndex.size() - 1];
		// nIndex 这个是循环开始
		CLoopPair loopPair;
		loopPair.nLoopStart = nSegIndex;
		loopPair.nLoopEnd = listRunningIndex[listRunningIndex.size() - 1];
		listLooper.push_back(loopPair);

		printf("循环节 %d --> %d\n", loopPair.nLoopStart, loopPair.nLoopEnd);

		for (unsigned int i = 0; i < listRunningIndex.size(); i++) {
			printf("->%d", listRunningIndex[i]);
		}
		printf("\n");
		// 循环节直接丢弃

		// 下次开始调试
		//while (1) {};
		return true;
	}

#if 1
	// 没有出现循环就添加进去
	listRunningIndex.push_back(nSegIndex);

	switch (listMethodSegment[nSegIndex].SegmentNextType) {
	case CJavaMethodSegment::nextNormal:		// 通过顺延过去
	{
		CString strNextLabel = SegmentNextGetOneLabel(listMethodSegment[nSegIndex].listStrNext, CString("next->"));
		int SSI = GetSegmentIndexFromName(strNextLabel);
		if (SSI >= 0) {
			GetThreadFullRunningListRecursion(SSI, listArrayRunningIndex, listRunningIndex);
		}
		else {
			printf("1无法找到函数中的起始位置\n");
		}
	}
	break;
	case CJavaMethodSegment::nextGoto:			// 通过跳转过去
	{
		CString strNextLabel = SegmentNextGetOneLabel(listMethodSegment[nSegIndex].listStrNext, CString("goto->"));
		int SSI = GetSegmentIndexFromName(strNextLabel);
		if (SSI >= 0) {
			GetThreadFullRunningListRecursion(SSI, listArrayRunningIndex, listRunningIndex);
		}
		else {
			printf("2无法找到函数中的起始位置\n");
		}
	}
	break;
	case CJavaMethodSegment::nextIf:			// 通过if过去, 不一定是if过去的，可能是 while for 循环
	{
		CString strIfyLabel = SegmentNextGetOneLabel(listMethodSegment[nSegIndex].listStrNext, CString("if-y->"));
		CString strIfnLabel = SegmentNextGetOneLabel(listMethodSegment[nSegIndex].listStrNext, CString("if-n->"));

		int SSI = GetSegmentIndexFromName(strIfyLabel);
		if (SSI >= 0) {
			GetThreadFullRunningListRecursion(SSI, listArrayRunningIndex, listRunningIndex);
		}
		else {
			printf("3无法找到函数中的起始位置\n");
		}
		SSI = GetSegmentIndexFromName(strIfnLabel);
		if (SSI >= 0) {
			GetThreadFullRunningListRecursion(SSI, listArrayRunningIndex, listRunningIndex);
		}
		else {
			printf("4无法找到函数中的起始位置\n");
		}
	}
	break;
	case CJavaMethodSegment::nextSwitch:		// 通过switch结构过去
	{

		CString strSwitchLabel = SegmentNextGetOneLabel(listMethodSegment[nSegIndex].listStrNext, CString("switch->"));
		CString strDefaultLabel = SegmentNextGetOneLabel(listMethodSegment[nSegIndex].listStrNext, CString("default->"));

		// 遍历 Switch 分支
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
						printf("5无法找到函数中的起始位置\n");
					}
				}
				break;
			}
		}

		// default 分支
		int SSI = GetSegmentIndexFromName(strDefaultLabel);
		if (SSI >= 0) {
			GetThreadFullRunningListRecursion(SSI, listArrayRunningIndex, listRunningIndex);
		}
		else {
			printf("6无法找到函数中的起始位置\n");
		}
	}
	break;
	case CJavaMethodSegment::nextReturn:		// 通过return返回的段
	{
		listArrayRunningIndex.push_back(listRunningIndex);
		return true;
	}
	break;
	case CJavaMethodSegment::nextTryCatch:		// 通过TryCatch分支
	{
		CString strTryLabel = SegmentNextGetOneLabel(listMethodSegment[nSegIndex].listStrNext, CString("try->"));
		CString strDefaultLabel = SegmentNextGetOneLabel(listMethodSegment[nSegIndex].listStrNext, CString("default->"));

		std::vector<CTryCatchNode>::iterator ittcn;
		for (ittcn = listCatchData.begin(); ittcn != listCatchData.end(); ittcn++) {
			std::vector<CCatchItemNode>::iterator itcit;
			if (strTryLabel.Compare((*ittcn).strName) == 0) {

				// try分支
				int SsiStart = GetSegmentIndexFromName((*ittcn).strStartLabel);
				if (SsiStart >= 0) {
					GetThreadFullRunningListRecursion(SsiStart, listArrayRunningIndex, listRunningIndex);
				}
				else {
					printf("181无法找到函数中的起始位置\n");
				}

				for (itcit = (*ittcn).listCatch.begin(); itcit != (*ittcn).listCatch.end(); itcit++) {
					int SSI = GetSegmentIndexFromName((*itcit).strCatchLabel);
					if (SSI >= 0) {
						GetThreadFullRunningListRecursion(SSI, listArrayRunningIndex, listRunningIndex);
					}
					else {
						printf("7无法找到函数中的起始位置\n");
					}
				}
				// final分支
				if (!(*ittcn).strFinallyLabel.IsEmpty()) {
					int SSI = GetSegmentIndexFromName((*ittcn).strFinallyLabel);
					if (SSI >= 0) {
						GetThreadFullRunningListRecursion(SSI, listArrayRunningIndex, listRunningIndex);
					}
					else {
						printf("8无法找到函数中的起始位置\n");
					}
				}

				break;
			}
		}

		// default 分支
		int SSI = GetSegmentIndexFromName(strDefaultLabel);
		if (SSI >= 0) {
			GetThreadFullRunningListRecursion(SSI, listArrayRunningIndex, listRunningIndex);
		}
		else {
			printf("9无法找到函数中的起始位置\n");
		}
	}
	break;
	case CJavaMethodSegment::nextThrow:			// 这个也是结束代码
	{
		listArrayRunningIndex.push_back(listRunningIndex);
		return true;
	}
	break;
	default:
	{
		printf("没有使用过的类型\n");
	}
	}
#endif

	return true;
}


std::vector<int> CJavaMethod::GetThreadFullRunningList(int nStartSegment, std::vector<int> listIndexSort, int nSegIndex) {

	std::vector<int> listRunningIndex;
	std::vector<std::vector<int>> listArrayRunningIndex;

	// 递归调用出来
	listRunningIndex.clear();
	GetThreadFullRunningListRecursion(nSegIndex, listArrayRunningIndex, listRunningIndex);

	for (unsigned int i = 0; i < listArrayRunningIndex.size(); i++) {
		printf("线程 %4d ：", i);
		for (unsigned int j = 0; j < listArrayRunningIndex[i].size(); j++) {
			printf("%4d ", listArrayRunningIndex[i][j]);
		}
		printf("\n");
	}


	// 排序，排成一张总表
	std::vector<std::vector<int>> listFullIndex;
	for (unsigned int i = 0; i < listArrayRunningIndex.size(); i++) {
		std::vector<int> listRunIndex;
		listRunIndex.clear();
		//printf("行序号 %d ：", i);
		for (unsigned int j = 0; j < listMethodSegment.size(); j++) {
			if (find(listArrayRunningIndex[i].begin(), listArrayRunningIndex[i].end(), j) != listArrayRunningIndex[i].end()) {
				listRunIndex.push_back(j);
				//printf("%4d,", j);				// 存在就有编号
			}
			else {
				listRunIndex.push_back(-1);
				//printf("%4d,", -1);				// 不存在就没有编号
			}
		}
		//printf("\n");
		listFullIndex.push_back(listRunIndex);
	}

	//==================================================>

	// 排过序号的
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
		printf("严重错误，排序出来的大小不一样");
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

	// 扣除掉自己的一项
	nStartIndex++;

	// 打印出来看看调试结果
	for (unsigned int i = 0; i < listFullIndexSort.size(); i++) {
		//printf("新排序 : ");
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

	printf("归一化的线序 ：");
	for (unsigned int i = 0; i < listMerge.size(); i++) {
		printf("%4d ", listMerge[i]);
	}
	printf("\n");

	return listMerge;
}

int CMarcoCodeItem::nSeed = 10000;

// 返回宏指令的序号
int CJavaMethod::MacroStructureRecursion(int nStartSeg, std::vector<int> listIndexSort, int nEndSeg) {

	CMarcoCodeItem  macroStruct;
	macroStruct.clear();

	// 当前函数的宏结构, 里面为结构体宏函数
	CString strMacroInst;

	int SSI = nStartSeg;


	// 循环边界
	while ((SSI >= 0) && (SSI != nEndSeg)) {

		switch (listMethodSegment[SSI].SegmentNextType) {
		case CJavaMethodSegment::nextNormal:		// 通过顺延过去
		{
			CString strNextLabel = SegmentNextGetOneLabel(listMethodSegment[SSI].listStrNext, CString("next->"));
			strMacroInst.Format("normal %d", SSI);
			macroStruct.listMacro.push_back(strMacroInst);
			SSI = GetSegmentIndexFromName(strNextLabel);
		}
		break;
		case CJavaMethodSegment::nextGoto:			// 通过跳转过去
		{
			CString strNextLabel = SegmentNextGetOneLabel(listMethodSegment[SSI].listStrNext, CString("goto->"));
			strMacroInst.Format("normal %d", SSI);
			macroStruct.listMacro.push_back(strMacroInst);
			SSI = GetSegmentIndexFromName(strNextLabel);
		}
		break;
		case CJavaMethodSegment::nextIf:			// 通过if过去, 不一定是if过去的，可能是 while for 循环
		{
			CString strIfyLabel = SegmentNextGetOneLabel(listMethodSegment[SSI].listStrNext, CString("if-y->"));
			CString strIfnLabel = SegmentNextGetOneLabel(listMethodSegment[SSI].listStrNext, CString("if-n->"));

			// 1、获得两个归一化的列表
			// 2、然后找到if的闭环位置
			// 3、根据闭环位置，统计两边不一致的位置
			int SsiIfyIndex = GetSegmentIndexFromName(strIfyLabel);
			int SsiIfnIndex = GetSegmentIndexFromName(strIfnLabel);
			std::vector<int> listRunningIfy = GetThreadFullRunningList(SSI, listIndexSort, SsiIfyIndex);
			std::vector<int> listRunningIfn = GetThreadFullRunningList(SSI, listIndexSort, SsiIfnIndex);

			if (listRunningIfy.size() != listRunningIfn.size()) {
				printf("严重错误，两个分支统计出来的大小不一样\n");
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

			printf("if 结构的结束位置 %d\n", nEndSegmentIndex);

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
				// SsiIfyIndex -> listRunningIfy[nOffset] 到这么一段， listRunningIfy[nOffset] 这个本省不能包含在内
				nMacroIfy = MacroStructureRecursion(SsiIfyIndex, listIndexSort, nEndSegmentIndex);
			}
			if (nIfnBlockCount > 0) {
				nMacroIfn = MacroStructureRecursion(SsiIfnIndex, listIndexSort, nEndSegmentIndex);
			}

			if ((nIfyBlockCount <= 0) && (nIfnBlockCount <= 0)) {
				// 空的 if 语句
				strMacroInst.Format("if %d,%d,%d", SSI, nMacroIfy, nMacroIfn);
				macroStruct.listMacro.push_back(strMacroInst);
			}
			else if ((nIfyBlockCount > 0) && (nIfnBlockCount <= 0)) {
				// yes 部分有， no 部分没有，需要翻转内容

				strMacroInst.Format("if %d,%d,%d", SSI, nMacroIfy, nMacroIfn);
				macroStruct.listMacro.push_back(strMacroInst);
			}
			else if ((nIfyBlockCount <= 0) && (nIfnBlockCount > 0)) {
				// no 部分有， yes 部分没有
				strMacroInst.Format("if %d,%d,%d", SSI, nMacroIfy, nMacroIfn);
				macroStruct.listMacro.push_back(strMacroInst);
			}
			else {
				// if 也有 else 也有
				strMacroInst.Format("if %d,%d,%d", SSI, nMacroIfy, nMacroIfn);
				macroStruct.listMacro.push_back(strMacroInst);
			}

			// 下一个SSS 的代码
			SSI = nEndSegmentIndex;
		}
		break;
		case CJavaMethodSegment::nextSwitch:		// 通过switch结构过去
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

					// 把 strDefaultLabel 加入
					int SsiStartIndex = GetSegmentIndexFromName(strDefaultLabel);
					std::vector<int> listRunning = GetThreadFullRunningList(SSI, listIndexSort, SsiStartIndex);
					listArrayRunningFullCase.push_back(listRunning);


					// 寻找每个段相似的位置
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
		case CJavaMethodSegment::nextReturn:		// 通过return返回的段
		{
			// 这个就是当前一段， listSegIndex 就是当前输出

			// 插入宏指令
			strMacroInst.Format("return %d", SSI);
			macroStruct.listMacro.push_back(strMacroInst);
			SSI = -1;

		}
		break;
		case CJavaMethodSegment::nextTryCatch:		// 通过TryCatch分支
		{
			CString strTryLabel = SegmentNextGetOneLabel(listMethodSegment[SSI].listStrNext, CString("try->"));
			CString strDefaultLabel = SegmentNextGetOneLabel(listMethodSegment[SSI].listStrNext, CString("default->"));


			int SsiDefaultIndex = GetSegmentIndexFromName(strDefaultLabel);

			for (unsigned int i = 0; i < this->listCatchData.size(); i++) {

				if (this->listCatchData[i].strName == strTryLabel) {

					// 这个是数据
					CString strName = listCatchData[i].strName;
					CString strStartLabel = listCatchData[i].strStartLabel;
					CString strEndLabel = listCatchData[i].strEndLabel;
					std::vector<CCatchItemNode> listCatch = listCatchData[i].listCatch;
					CString strFinallyLabel = listCatchData[i].strFinallyLabel;

					std::vector<std::vector<int>> listArrayRunningFullCatch;
					listArrayRunningFullCatch.clear();

					// 把 catch 加入
					for (unsigned int j = 0; j < listCatch.size(); j++) {
						int SsiCatchIndex = GetSegmentIndexFromName(listCatch[j].strCatchLabel);
						std::vector<int> listRunning = GetThreadFullRunningList(SSI, listIndexSort, SsiCatchIndex);
						listArrayRunningFullCatch.push_back(listRunning);
					}
					// 把 strStartLabel 加入
					int SsiStartIndex = GetSegmentIndexFromName(strStartLabel);
					std::vector<int> listRunning = GetThreadFullRunningList(SSI, listIndexSort, SsiStartIndex);
					listArrayRunningFullCatch.push_back(listRunning);

					// 寻找每个段相似的位置
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
						// Catch 的退出位置不一致的情况
						printf("Catch 的退出位置不一致的情况\n");
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
		case CJavaMethodSegment::nextThrow:			// 这个也是结束代码
		{
			// 插入宏指令
			strMacroInst.Format("throw %d", SSI);
			macroStruct.listMacro.push_back(strMacroInst);
			SSI = -1;
		}
		break;
		default:
		{
			printf("没有使用过的类型\n");
		}
		}

		// 终止条件判断
	}


	// 放入队列返回
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
					printf("01不能识别的宏指令 %s\n", strMacro);
					while (1) {};
				}

			}

			return;
		}
	}

	printf("找不到的宏结构");

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
				printf("找不到序号 %4d 对应的指令\n", nInstIndex);
				while (1) {};
			}

			if (it->strInstString.Find("catch") != 0) {
				printf("这个指令不是 catch 指令\n");
				while (1) {};
			}

			return it->regInfo.outReg.strValName;
		}
	}

	return CString("Unknow!");
}

// 递归显示宏结构
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

					// 解析指令
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
					// 解析指令
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
					// 解析指令
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
					// 解析指令
					SegmentCodeTranslate(nLevel, listOldInstList, listCurrentInstIndex);
					listOldInstList.insert(listOldInstList.end(), listCurrentInstIndex.begin(), listCurrentInstIndex.end());

					int nLastInstIndex = listCurrentInstIndex[listCurrentInstIndex.size() - 1];

					std::vector<CJavaInstructItem>::iterator itLastInst = GetOriginalInstruct(nLastInstIndex);
					if (itLastInst == this->listOriginalInstruct.end()) {
						printf("找不到的指令\n");
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
					// 解析指令
					SegmentCodeTranslate(nLevel, listOldInstList, listCurrentInstIndex);
					listOldInstList.insert(listOldInstList.end(), listCurrentInstIndex.begin(), listCurrentInstIndex.end());

					int nLastInstIndex = listCurrentInstIndex[listCurrentInstIndex.size() - 1];

					std::vector<CJavaInstructItem>::iterator itLastInst = GetOriginalInstruct(nLastInstIndex);
					if (itLastInst == this->listOriginalInstruct.end()) {
						printf("找不到的指令\n");
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
						// 这里要获得 catch 的变量
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
					// 解析指令
					SegmentCodeTranslate(nLevel, listOldInstList, listCurrentInstIndex);
					listOldInstList.insert(listOldInstList.end(), listCurrentInstIndex.begin(), listCurrentInstIndex.end());

					int nLastInstIndex = listCurrentInstIndex[listCurrentInstIndex.size() - 1];

					std::vector<CJavaInstructItem>::iterator itLastInst = GetOriginalInstruct(nLastInstIndex);
					if (itLastInst == this->listOriginalInstruct.end()) {
						printf("找不到的指令\n");
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
					printf("02不能识别的宏指令 %s\n", strMacro);
					while (1) {};
				}

			}

			return;
		}
	}

	printf("找不到的宏结构");

}


// .method public static saveImageToGallery(Landroid/content/Context;Landroid/graphics/Bitmap;Ljava/lang/String;)V
// 递归的方式，找出所有可能的运行路径，保存在listArrayRunningIndex中
//                                            用于保存临时路径的队列        全局变量                                              开始的段索引
bool CJavaMethod::GetFullRunVectorRecursion(std::vector<int> listSegIndex, std::vector<std::vector<int>> & listArrayRunningIndex, int nIndex) {

	if (find(listSegIndex.begin(), listSegIndex.end(), nIndex) != listSegIndex.end()) {
		listArrayRunningIndex.push_back(listSegIndex);
		return true;
	}

	// 当前层添加进去
	listSegIndex.push_back(nIndex);

	switch (listMethodSegment[nIndex].SegmentNextType) {
	case CJavaMethodSegment::nextNormal:		// 通过顺延过去
	{
		CString strNextLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("next->"));
		int SSI = GetSegmentIndexFromName(strNextLabel);
		if (SSI >= 0) {
			GetFullRunVectorRecursion(listSegIndex, listArrayRunningIndex, SSI);
		}
		else {
			printf("1无法找到函数中的起始位置\n");
		}
	}
	break;
	case CJavaMethodSegment::nextGoto:			// 通过跳转过去
	{
		CString strNextLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("goto->"));
		int SSI = GetSegmentIndexFromName(strNextLabel);
		if (SSI >= 0) {
			GetFullRunVectorRecursion(listSegIndex, listArrayRunningIndex, SSI);
		}
		else {
			printf("2无法找到函数中的起始位置\n");
		}
	}
	break;
	case CJavaMethodSegment::nextIf:			// 通过if过去, 不一定是if过去的，可能是 while for 循环
	{
		CString strIfyLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("if-y->"));
		CString strIfnLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("if-n->"));

		int SSI = GetSegmentIndexFromName(strIfyLabel);
		if (SSI >= 0) {
			GetFullRunVectorRecursion(listSegIndex, listArrayRunningIndex, SSI);
		}
		else {
			printf("3无法找到函数中的起始位置\n");
		}
		SSI = GetSegmentIndexFromName(strIfnLabel);
		if (SSI >= 0) {
			GetFullRunVectorRecursion(listSegIndex, listArrayRunningIndex, SSI);
		}
		else {
			printf("4无法找到函数中的起始位置\n");
		}
	}
	break;
	case CJavaMethodSegment::nextSwitch:		// 通过switch结构过去
	{

		CString strSwitchLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("switch->"));
		CString strDefaultLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("default->"));

		// 遍历 Switch 分支
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
						printf("5无法找到函数中的起始位置\n");
					}
				}
				break;
			}
		}

		// default 分支
		int SSI = GetSegmentIndexFromName(strDefaultLabel);
		if (SSI >= 0) {
			GetFullRunVectorRecursion(listSegIndex, listArrayRunningIndex, SSI);
		}
		else {
			printf("6无法找到函数中的起始位置\n");
		}
	}
	break;
	case CJavaMethodSegment::nextReturn:		// 通过return返回的段
	{
		listArrayRunningIndex.push_back(listSegIndex);
		return true;
	}
	break;
	case CJavaMethodSegment::nextTryCatch:		// 通过TryCatch分支
	{
		CString strTryLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("try->"));
		CString strDefaultLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("default->"));

		std::vector<CTryCatchNode>::iterator ittcn;
		for (ittcn = listCatchData.begin(); ittcn != listCatchData.end(); ittcn++) {
			std::vector<CCatchItemNode>::iterator itcit;
			if (strTryLabel.Compare((*ittcn).strName) == 0) {

				// try 分支
				int SsiStart = GetSegmentIndexFromName((*ittcn).strStartLabel);
				if (SsiStart >= 0) {
					GetFullRunVectorRecursion(listSegIndex, listArrayRunningIndex, SsiStart);
				}
				else {
					printf("181无法找到函数中的起始位置\n");
				}

				// catch 分支
				for (itcit = (*ittcn).listCatch.begin(); itcit != (*ittcn).listCatch.end(); itcit++) {
					int SSI = GetSegmentIndexFromName((*itcit).strCatchLabel);
					if (SSI >= 0) {
						GetFullRunVectorRecursion(listSegIndex, listArrayRunningIndex, SSI);
					}
					else {
						printf("7无法找到函数中的起始位置\n");
					}
				}
				// final 分支
				if (!(*ittcn).strFinallyLabel.IsEmpty()) {
					int SSI = GetSegmentIndexFromName((*ittcn).strFinallyLabel);
					if (SSI >= 0) {
						GetFullRunVectorRecursion(listSegIndex, listArrayRunningIndex, SSI);
					}
					else {
						printf("8无法找到函数中的起始位置\n");
					}
				}

				break;
			}
		}

		// default 分支
		int SSI = GetSegmentIndexFromName(strDefaultLabel);
		if (SSI >= 0) {
			GetFullRunVectorRecursion(listSegIndex, listArrayRunningIndex, SSI);
		}
		else {
			printf("9无法找到函数中的起始位置\n");
		}
	}
	break;
	case CJavaMethodSegment::nextThrow:			// 这个也是结束代码
	{
		listArrayRunningIndex.push_back(listSegIndex);
		return true;
	}
	break;
	default:
	{
		printf("没有使用过的类型\n");
	}
	}

	return true;
}


// 更加精细的计算运行线程，解决之前遇到的问题
int CJavaMethod::CreateMacroCodeStruction(void) {
	std::vector<int> listSegIndex;
	listSegIndex.clear();

	CMarcoCodeItem::ResetSeed();
	listMarco.clear();

	// 下面一段开始进行结构体切割
	//listSegIndex.clear();
	int SSI = GetSegmentIndexFromName(CString("main"));

	if (SSI >= 0) {

		int nMacroIndex = MacroStructureRecursion(SSI, listFullRunVector, 1000000);

		printf("显示宏结构\n");
		ShowAbsMacroStructRecursion(listSegIndex, 0, nMacroIndex);

		return nMacroIndex;


	}

	return -1;
}

// 开始翻译宏结构
void CJavaMethod::TranslateMacroCodeStruct(int nMacroIndex) {
	std::vector<int> listSegIndex;
	listSegIndex.clear();

	// 这个函数开始翻译
	printf("显示翻译\n");
	if (nMacroIndex > 0) {
		TranslateMacroStructRecursion(listSegIndex, 0, nMacroIndex);
	}
}



