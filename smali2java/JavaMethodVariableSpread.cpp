#include "stdafx.h"
#include "JavaClass.h"
#include "JavaMethod.h"
#include "JavaInstructItem.h"

#include <algorithm>

// 递归，然后进行变量名称命名
bool CJavaMethod::SegmentVariableSpreadRecursion(std::vector<int> listSegIndex, int nIndex) {

	if (find(listSegIndex.begin(), listSegIndex.end(), nIndex) != listSegIndex.end()) {
		// 这里是判断进入了循环
		// 这个是最后一个序号：listSegIndex[listSegIndex.size() - 1];
		// nIndex 这个是循环开始
		CLoopPair loopPair;
		loopPair.nLoopStart = nIndex;
		loopPair.nLoopEnd = listSegIndex[listSegIndex.size() - 1];

		printf("循环节 %d --> %d\n", loopPair.nLoopStart, loopPair.nLoopEnd);

		// 下次开始调试
		while (1) {};
		return true;
	}

	// 没有出现循环就添加进去
	listSegIndex.push_back(nIndex);

	switch (listMethodSegment[nIndex].SegmentNextType) {
	case CJavaMethodSegment::nextNormal:		// 通过顺延过去
	{
		CString strNextLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("next->"));
		int SSI = GetSegmentIndexFromName(strNextLabel);
		if (SSI >= 0) {
			SegmentVariableSpreadRecursion(listSegIndex, SSI);
		}
		else {
			printf("12无法找到函数中的起始位置\n");
		}
	}
	break;
	case CJavaMethodSegment::nextGoto:			// 通过跳转过去
	{
		CString strNextLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("goto->"));
		int SSI = GetSegmentIndexFromName(strNextLabel);
		if (SSI >= 0) {
			SegmentVariableSpreadRecursion(listSegIndex, SSI);
		}
		else {
			printf("13无法找到函数中的起始位置\n");
		}
	}
	break;
	case CJavaMethodSegment::nextIf:			// 通过if过去, 不一定是if过去的，可能是 while for 循环
	{
		CString strIfyLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("if-y->"));
		CString strIfnLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("if-n->"));

		int SSI = GetSegmentIndexFromName(strIfyLabel);
		if (SSI >= 0) {
			SegmentVariableSpreadRecursion(listSegIndex, SSI);
		}
		else {
			printf("14无法找到函数中的起始位置\n");
		}
		SSI = GetSegmentIndexFromName(strIfnLabel);
		if (SSI >= 0) {
			SegmentVariableSpreadRecursion(listSegIndex, SSI);
		}
		else {
			printf("41无法找到函数中的起始位置\n");
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
						SegmentVariableSpreadRecursion(listSegIndex, SSI);
					}
					else {
						printf("16无法找到函数中的起始位置\n");
					}
				}
				break;
			}
		}

		// default 分支
		int SSI = GetSegmentIndexFromName(strDefaultLabel);
		if (SSI >= 0) {
			SegmentVariableSpreadRecursion(listSegIndex, SSI);
		}
		else {
			printf("17无法找到函数中的起始位置\n");
		}
	}
	break;
	case CJavaMethodSegment::nextReturn:		// 通过return返回的段
	{
		return SegmentListInstSpread(listSegIndex);
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

				// try分支
				int SsiStart = GetSegmentIndexFromName((*ittcn).strStartLabel);
				if (SsiStart >= 0) {
					SegmentVariableSpreadRecursion(listSegIndex, SsiStart);
				}
				else {
					printf("181无法找到函数中的起始位置\n");
				}

				for (itcit = (*ittcn).listCatch.begin(); itcit != (*ittcn).listCatch.end(); itcit++) {
					int SSI = GetSegmentIndexFromName((*itcit).strCatchLabel);
					if (SSI >= 0) {
						SegmentVariableSpreadRecursion(listSegIndex, SSI);
					}
					else {
						printf("18无法找到函数中的起始位置\n");
					}
				}
				// final分支
				if (!(*ittcn).strFinallyLabel.IsEmpty()) {
					int SSI = GetSegmentIndexFromName((*ittcn).strFinallyLabel);
					if (SSI >= 0) {
						SegmentVariableSpreadRecursion(listSegIndex, SSI);
					}
					else {
						printf("19无法找到函数中的起始位置\n");
					}
				}

				break;
			}
		}

		// default 分支
		int SSI = GetSegmentIndexFromName(strDefaultLabel);
		if (SSI >= 0) {
			SegmentVariableSpreadRecursion(listSegIndex, SSI);
		}
		else {
			printf("20无法找到函数中的起始位置\n");
		}
	}
	break;
	case CJavaMethodSegment::nextThrow:			// 这个也是结束代码
	{
		// 调用到尾巴上
		return SegmentListInstSpread(listSegIndex);
	}
	break;
	default:
	{
		printf("没有使用过的类型\n");
	}
	}

	return true;
}


// 通过递归方式反演出所有寄存器可能的类型
void CJavaMethod::SegmentVariableSpread(void) {

	std::vector<int> listSegIndex;

	// 变量传播
	listSegIndex.clear();
	int SSI = GetSegmentIndexFromName(CString("main"));
	if (SSI >= 0) {
		SegmentVariableSpreadRecursion(listSegIndex, SSI);
	}
	else {
		printf("21无法找到函数中的起始位置\n");
	}

	// 检测是否所有变量都有名字
	for (unsigned int i = 0; i < this->listOriginalInstruct.size(); i++) {
		if (this->listOriginalInstruct[i].regInfo.listIreg.size() > 0) {	// 有数据寄存器，那么就有资格对输出寄存器分组
			for (unsigned int j = 0; j < this->listOriginalInstruct[i].regInfo.listIreg.size(); j++) {
				if (this->listOriginalInstruct[i].regInfo.listIreg[j].strValName.IsEmpty()) {
					printf("还有输入变量没有命名 %4d %s\n", this->listOriginalInstruct[i].nSerialNumber, this->listOriginalInstruct[i].strInstString);
					while (1) {};
				}
			}
			if (!this->listOriginalInstruct[i].regInfo.outReg.strRegName.IsEmpty()) {

				// 被使用的才有名字
				if (this->listOriginalInstruct[i].regInfo.outReg.listQuotePosition.size() > 0) {
					if (this->listOriginalInstruct[i].regInfo.outReg.strValName.IsEmpty()) {
						printf("还有输出变量没有命名 %4d %s\n", this->listOriginalInstruct[i].nSerialNumber, this->listOriginalInstruct[i].strInstString);
						while (1) {};
					}
				}
			}
		}
	}
}