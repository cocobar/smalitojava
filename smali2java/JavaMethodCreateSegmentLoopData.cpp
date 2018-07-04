#include "stdafx.h"
#include "JavaClass.h"
#include "JavaMethod.h"
#include "JavaInstructItem.h"

#include <algorithm>


// .method public static saveImageToGallery(Landroid/content/Context;Landroid/graphics/Bitmap;Ljava/lang/String;)V
// 这个函数有循环节
bool CJavaMethod::SegmentLoopRecursion(std::vector<int> listSegIndex, int nIndex) {

	if (find(listSegIndex.begin(), listSegIndex.end(), nIndex) != listSegIndex.end()) {
		// 这里是判断进入了循环
		// 这个是最后一个序号：listSegIndex[listSegIndex.size() - 1];
		// nIndex 这个是循环开始
		CLoopPair loopPair;
		loopPair.nLoopStart = nIndex;
		loopPair.nLoopEnd = listSegIndex[listSegIndex.size() - 1];
		listLooper.push_back(loopPair);

		printf("循环节 %d --> %d\n", loopPair.nLoopStart, loopPair.nLoopEnd);

		for (unsigned int i = 0; i < listSegIndex.size(); i++) {
			printf("->%d", listSegIndex[i]);
		}
		printf("\n");


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
			SegmentLoopRecursion(listSegIndex, SSI);
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
			SegmentLoopRecursion(listSegIndex, SSI);
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
			SegmentLoopRecursion(listSegIndex, SSI);
		}
		else {
			printf("3无法找到函数中的起始位置\n");
		}
		SSI = GetSegmentIndexFromName(strIfnLabel);
		if (SSI >= 0) {
			SegmentLoopRecursion(listSegIndex, SSI);
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
						SegmentLoopRecursion(listSegIndex, SSI);
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
			SegmentLoopRecursion(listSegIndex, SSI);
		}
		else {
			printf("6无法找到函数中的起始位置\n");
		}
	}
	break;
	case CJavaMethodSegment::nextReturn:		// 通过return返回的段
	{
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


				// try分支
				int SsiStart = GetSegmentIndexFromName((*ittcn).strStartLabel);
				if (SsiStart >= 0) {
					SegmentLoopRecursion(listSegIndex, SsiStart);
				}
				else {
					printf("181无法找到函数中的起始位置\n");
				}

				for (itcit = (*ittcn).listCatch.begin(); itcit != (*ittcn).listCatch.end(); itcit++) {
					int SSI = GetSegmentIndexFromName((*itcit).strCatchLabel);
					if (SSI >= 0) {
						SegmentLoopRecursion(listSegIndex, SSI);
					}
					else {
						printf("7无法找到函数中的起始位置\n");
					}
				}
				// final分支
				if (!(*ittcn).strFinallyLabel.IsEmpty()) {
					int SSI = GetSegmentIndexFromName((*ittcn).strFinallyLabel);
					if (SSI >= 0) {
						SegmentLoopRecursion(listSegIndex, SSI);
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
			SegmentLoopRecursion(listSegIndex, SSI);
		}
		else {
			printf("9无法找到函数中的起始位置\n");
		}
	}
	break;
	case CJavaMethodSegment::nextThrow:			// 这个也是结束代码
	{
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



// 递归识别出所有的LOOP
void CJavaMethod::CreateSegmentLoopData(void) {
	std::vector<int> listSegIndex;

	int SSI = GetSegmentIndexFromName(CString("main"));

	if (SSI >= 0) {
		SegmentLoopRecursion(listSegIndex, SSI);
	}
	else {
		printf("10无法找到函数中的起始位置\n");
	}
}
