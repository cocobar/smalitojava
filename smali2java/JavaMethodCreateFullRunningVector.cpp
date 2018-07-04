#include "stdafx.h"
#include "JavaClass.h"
#include "JavaMethod.h"
#include "JavaInstructItem.h"

#include <algorithm>

static int SortNumberCmp(const CSortNumber & a, const CSortNumber &b)
{
	return (a.nKey < b.nKey);
}


std::vector<int> CJavaMethod::GetFullRunVector(void) {
	std::vector<int> listRunVector;
	std::vector<int> listRunningIndex;
	std::vector<std::vector<int>> listArrayRunningIndex;
	listRunVector.clear();

	int SSI = GetSegmentIndexFromName(CString("main"));

	if (SSI >= 0) {
		GetFullRunVectorRecursion(listRunningIndex, listArrayRunningIndex, SSI);

		// 排序，排成一张总表
		std::vector<std::vector<int>> listFullIndex;
		for (unsigned int i = 0; i < listArrayRunningIndex.size(); i++) {
			std::vector<int> listRunIndex;
			listRunIndex.clear();
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
			listFullIndex.push_back(listRunIndex);
		}

		//listFullIndex 里面保存的是每个线程可以运行哪些代码的情况 -1 -1 2

		// 使用新的算法，将 listFullIndex 线序重新调整，调整和执行线序一致
		// 0 3 7 9 2
		// 0 3 7 8 9 2
		// 0 3 4 6 7 9 2
		// 0 3 4 6 7 8 9 2
		// 0 1 2

		// 生成排序表
		std::vector<CSortNumber> listIndexSort;
		listIndexSort.clear();
		for (unsigned int j = 0; j < listMethodSegment.size(); j++) {
			CSortNumber cSort;
			cSort.nData = j;
			cSort.nKey = j;
			listIndexSort.push_back(cSort);		// 这里先不进行排序，查找速度会快一点
		}

		// 开始自己增加排序的序号，类似冒泡法
		std::vector<CSortNumber>::iterator itSort;
		int nDoChangeCount = 1;
		int nWhileLoopCount = 0;
		while (nDoChangeCount > 0) {
			nDoChangeCount = 0;
			if (nWhileLoopCount++ > 10000) {
				printf("这个算法有问题，可能出现死循环了\n");
				break;
			}

			for (itSort = listIndexSort.begin(); itSort < listIndexSort.end(); itSort++) {
				for (unsigned int i = 0; i < listArrayRunningIndex.size(); i++) {
					for (unsigned int j = 0; j < listArrayRunningIndex[i].size(); j++) {

						// 找到这个数据,然后找前面的数据，然后依次循环，我这里节点永远要比前面出现的节点的序号大
						if ((listArrayRunningIndex[i])[j] == (*itSort).nData) {
							for (unsigned int k = 0; k < j; k++) {
								if ((*itSort).nKey <= listIndexSort[(listArrayRunningIndex[i])[k]].nKey) {
									(*itSort).nKey = (listIndexSort[(listArrayRunningIndex[i])[k]].nKey + 1);
									nDoChangeCount++;
								}
							}
						}
					}
				}
			}
		}

		std::sort(listIndexSort.begin(), listIndexSort.end(), SortNumberCmp);

#if 1
		printf("全局排序出来的数据 : ");
		for (itSort = listIndexSort.begin(); itSort < listIndexSort.end(); itSort++) {
			printf("-> %d ", (*itSort).nData);
			listRunVector.push_back((*itSort).nData);
		}
		printf("\n");
#endif

	}
	else {
		printf("10无法找到函数中的起始位置\n");
	}


	return listRunVector;
}
