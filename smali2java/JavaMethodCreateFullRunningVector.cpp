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

		// �����ų�һ���ܱ�
		std::vector<std::vector<int>> listFullIndex;
		for (unsigned int i = 0; i < listArrayRunningIndex.size(); i++) {
			std::vector<int> listRunIndex;
			listRunIndex.clear();
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
			listFullIndex.push_back(listRunIndex);
		}

		//listFullIndex ���汣�����ÿ���߳̿���������Щ�������� -1 -1 2

		// ʹ���µ��㷨���� listFullIndex �������µ�����������ִ������һ��
		// 0 3 7 9 2
		// 0 3 7 8 9 2
		// 0 3 4 6 7 9 2
		// 0 3 4 6 7 8 9 2
		// 0 1 2

		// ���������
		std::vector<CSortNumber> listIndexSort;
		listIndexSort.clear();
		for (unsigned int j = 0; j < listMethodSegment.size(); j++) {
			CSortNumber cSort;
			cSort.nData = j;
			cSort.nKey = j;
			listIndexSort.push_back(cSort);		// �����Ȳ��������򣬲����ٶȻ��һ��
		}

		// ��ʼ�Լ������������ţ�����ð�ݷ�
		std::vector<CSortNumber>::iterator itSort;
		int nDoChangeCount = 1;
		int nWhileLoopCount = 0;
		while (nDoChangeCount > 0) {
			nDoChangeCount = 0;
			if (nWhileLoopCount++ > 10000) {
				printf("����㷨�����⣬���ܳ�����ѭ����\n");
				break;
			}

			for (itSort = listIndexSort.begin(); itSort < listIndexSort.end(); itSort++) {
				for (unsigned int i = 0; i < listArrayRunningIndex.size(); i++) {
					for (unsigned int j = 0; j < listArrayRunningIndex[i].size(); j++) {

						// �ҵ��������,Ȼ����ǰ������ݣ�Ȼ������ѭ����������ڵ���ԶҪ��ǰ����ֵĽڵ����Ŵ�
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
		printf("ȫ��������������� : ");
		for (itSort = listIndexSort.begin(); itSort < listIndexSort.end(); itSort++) {
			printf("-> %d ", (*itSort).nData);
			listRunVector.push_back((*itSort).nData);
		}
		printf("\n");
#endif

	}
	else {
		printf("10�޷��ҵ������е���ʼλ��\n");
	}


	return listRunVector;
}
