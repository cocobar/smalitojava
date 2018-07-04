#include "stdafx.h"
#include "JavaClass.h"
#include "JavaMethod.h"
#include "JavaInstructItem.h"

#include <algorithm>


// 这个函数的作用就是将整个Method的代码全部分段
void CJavaMethod::CreateSegmentList(std::vector<CString> listMethodInst)
{
	std::vector<CString>::iterator it;
	std::vector<CString> listSwitchData;
	int nLineNumber = 0;

	CJavaMethodSegment instSegment;

	listCatchData.clear();
	listSwitchData.clear();
	instSegment.clear();
	instSegment.listSegName.push_back(CString("main"));

	// 优先扫描一次.catch .catchall
	int tmpIndex = 0;
	for (it = listMethodInst.begin(), tmpIndex++; it != listMethodInst.end(); it++, tmpIndex++) {
		CString strLine = (*it);
		CString strInstString = strLine;

		std::vector<CString> listI = CJavaInstructItem::GetListInstructionSymbol(strInstString);

		// .catchall {:try_start_0 .. :try_end_0} :catchall_0
		// .catch Ljava/io/IOException; {:try_start_0 .. :try_end_0} :catch_0

		if ((listI[0].Compare(".catch") == 0) || (listI[0].Compare(".catchall") == 0)) {

			CTryCatchNode tryCatchNode;

			tryCatchNode.clear();
			int nFindDotPost = strLine.Find("}");
			CString strLabel = strLine.Right(strLine.GetLength() - nFindDotPost - 1).Trim();			// 这个是catch之后执行的标签

																										// 取得中间{xxxx} 的内容
			strLine = strLine.Left(nFindDotPost);
			nFindDotPost = strLine.Find("{");
			strLine = strLine.Right(strLine.GetLength() - nFindDotPost - 1);

			nFindDotPost = strLine.Find("..");
			CString strStartLabel = strLine.Left(nFindDotPost).Trim();									// 开始标签
			CString strEndLabel = strLine.Right(strLine.GetLength() - nFindDotPost - 2).Trim();			// 结束标签

			int nFindTryCatchIndex = -1;
			for (unsigned int i = 0; i < listCatchData.size(); i++) {
				if ((listCatchData[i].strStartLabel.Compare(strStartLabel) == 0) || (listCatchData[i].strEndLabel.Compare(strEndLabel) == 0)) {
					nFindTryCatchIndex = i;
					break;
				}
			}

			if (nFindTryCatchIndex >= 0) {		// 已经存在了
				if ((listI[0].Compare(".catch") == 0)) {
					CCatchItemNode cCatchItemNode;
					nFindDotPost = listI[1].Find("{");
					CString strType = CJavaClass::GetCppTypeFromJava(listI[1].Left(nFindDotPost).Trim());
					cCatchItemNode.strCatchType = strType;
					cCatchItemNode.strCatchLabel = strLabel;
					listCatchData[nFindTryCatchIndex].listCatch.push_back(cCatchItemNode);
				}
				else if ((listI[0].Compare(".catchall") == 0)) {
					if (listCatchData[nFindTryCatchIndex].strFinallyLabel.IsEmpty()) {
						listCatchData[nFindTryCatchIndex].strFinallyLabel = strLabel;
					}
					else {
						printf("有多个finally出口\n");
						while (1) {};
					}
				}
			}
			else {

				tryCatchNode.clear();
				tryCatchNode.strStartLabel = strStartLabel;
				tryCatchNode.strEndLabel = strEndLabel;
				tryCatchNode.strName.Format("VirtualTry%d", tmpIndex);


				if ((listI[0].Compare(".catch") == 0)) {
					CCatchItemNode cCatchItemNode;
					nFindDotPost = listI[1].Find("{");
					CString strType = CJavaClass::GetCppTypeFromJava(listI[1].Left(nFindDotPost).Trim());
					cCatchItemNode.strCatchType = strType;
					cCatchItemNode.strCatchLabel = strLabel;

					tryCatchNode.listCatch.push_back(cCatchItemNode);
				}
				else if ((listI[0].Compare(".catchall") == 0)) {
					if (listCatchData[nFindTryCatchIndex].strFinallyLabel.IsEmpty()) {
						tryCatchNode.strFinallyLabel = strLabel;
					}
					else {
						printf("有多个finally出口\n");
						while (1) {};
					}
				}
				listCatchData.push_back(tryCatchNode);
			}
		}
	}

	// 再扫描一次switch结构
	for (it = listMethodInst.begin(); it != listMethodInst.end(); it++) {
		CString strLine = (*it);
		if ((strLine.Find("sparse-switch") == 0) || (strLine.Find("packed-switch") == 0)) {		// 属于前段
			int nDotFind = strLine.Find(":");
			CString strSwitchData = strLine.Right(strLine.GetLength() - nDotFind);
			strSwitchData.Trim();
			listSwitchData.push_back(strSwitchData);
		}
		else if (strLine.Find(":") == 0) {
			// 判断是否是 switch 架构
			bool bFindSwitch = false;
			if (listSwitchData.size() > 0) {
				std::vector<CString>::iterator switchit;
				for (switchit = listSwitchData.begin(); switchit != listSwitchData.end(); switchit++) {
					if (strLine.Compare(*switchit) == 0) {
						bFindSwitch = true;
						break;
					}
				}
			}
			if (bFindSwitch) {
				bool bPackedSwitch = false;
				bool bSparseSwitch = false;
				unsigned int nPackedSwitchData = 0;
				CJavaPackedSwitchStruct packedSwitchStruct;
				packedSwitchStruct.strSwitchName = strLine;

				for (it++, nLineNumber++; it != listMethodInst.end(); it++, nLineNumber++) {
					strLine = (*it);
					if (strLine.Find(".packed-switch") == 0) {
						// 开始
						CString strData = strLine.Right(strLine.GetLength() - CString(".packed-switch").GetLength());
						strData.Trim();
						bPackedSwitch = true;
						nPackedSwitchData = GetDataFromHex(strData);
					}
					else if (strLine.Find(".end packed-switch") == 0) {
						// 结束
						if (!bPackedSwitch) {
							printf("格式有错误 %s %d\n", __FILE__, __LINE__);
						}
						else {
							listPackedSwitch.push_back(packedSwitchStruct);
						}
						break;
					}
					else if (strLine.Find(".sparse-switch") == 0) {
						// 开始
						bSparseSwitch = true;
					}
					else if (strLine.Find(".end sparse-switch") == 0) {
						if (!bSparseSwitch) {
							printf("格式有错误 %s %d\n", __FILE__, __LINE__);
						}
						else {
							listPackedSwitch.push_back(packedSwitchStruct);
						}
						break;
					}
					else {
						if (bPackedSwitch && !bSparseSwitch) {
							if (strLine.Find(":") == 0) {
								CJavaSwitchItem switchItem;
								switchItem.nData = nPackedSwitchData++;
								switchItem.strSegmentName = strLine;
								packedSwitchStruct.listItem.push_back(switchItem);
							}
							else {
								printf("格式有错误 %s %d\n", __FILE__, __LINE__);
							}
						}
						else if (!bPackedSwitch && bSparseSwitch) {
							CJavaSwitchItem switchItem;
							unsigned int nData = 0;
							CString strLableName;
							int nFindDot = strLine.Find("->");
							if (nFindDot > 0) {
								nData = GetDataFromHex(strLine.Left(nFindDot));
							}
							nFindDot = strLine.Find(":");
							if (nFindDot > 0) {
								strLableName = strLine.Right(strLine.GetLength() - nFindDot);
							}
							switchItem.nData = nData;						// 行标数据
							switchItem.strSegmentName = strLableName;		// 对应的名称
							packedSwitchStruct.listItem.push_back(switchItem);
						}
						else {
							printf("格式有错误 %s %d\n", __FILE__, __LINE__);
						}
					}
				}
			}
		}
	}

	// 插入 try 结构指令
	for (it = listMethodInst.begin(); it != listMethodInst.end(); it++) {
		CString strLine = (*it);
		if (strLine.Find(":") == 0) {
			strLine.Trim();
			for (unsigned int i = 0; i < listCatchData.size(); i++) {
				if (listCatchData[i].strStartLabel.Compare(strLine) == 0) {
					CString strTmpInst;
					strTmpInst.Format("try %s, %s", listCatchData[i].strName, listCatchData[i].strEndLabel);
					it = listMethodInst.insert(it, strTmpInst);
					it++;
					break;
				}
			}
		}
	}

	// 插入 catch 结构
	for (it = listMethodInst.begin(); it != listMethodInst.end(); it++) {
		CString strLine = (*it);
		if (strLine.Find(":") == 0) {
			strLine.Trim();
			for (unsigned int i = 0; i < listCatchData.size(); i++) {
				for (unsigned int j = 0; j < listCatchData[i].listCatch.size(); j++) {
					if (listCatchData[i].listCatch[j].strCatchLabel.Compare(strLine) == 0) {
						CString strTmpInst;
						strTmpInst.Format("catch %s, %s", CString("e0"), listCatchData[i].listCatch[j].strCatchType);
						it++;
						it = listMethodInst.insert(it, strTmpInst);
						break;
					}
				}
			}
		}
	}

	for (it = listMethodInst.begin(); it != listMethodInst.end(); it++, nLineNumber++) {
		CString strLineName;
		strLineName.Format("line%d", nLineNumber);
		CString strLine = (*it);

		if (strLine.Find(".method") == 0) {
			// 函数名称
			this->strMethodName = this->GetMethodName(strLine);
			this->strReturnType = this->GetReturnTypeFromMethods(strLine);
			this->listMethodAttr = this->GetMethodAttribute(strLine);
			this->listParamType = this->GetSignatureFromMethods(strLine);

			// 保存Method信息
			this->strMethodInfo = strLine;

			// 这里要生成参数列表
			listInputParam.clear();
			int nPStart = 1;
			if (find(listMethodAttr.begin(), listMethodAttr.end(), CString("static")) != listMethodAttr.end()) {
				nPStart = 0;
			}
			else {
				CMethodParam cMethodParam;
				cMethodParam.strReg = CString("p0");
				cMethodParam.strTypeName = CString("this");
				cMethodParam.strValueName = CString("this");
				listInputParam.push_back(cMethodParam);
			}


			for (unsigned int i = 0; i < listParamType.size(); i++) {

				CMethodParam cMethodParam;

				cMethodParam.strReg.Format("p%d", nPStart);
				cMethodParam.strTypeName = listParamType[i];
				cMethodParam.strValueName.Format("paramP%d", nPStart);

				if ((listParamType[i].Compare("long") == 0) || (listParamType[i].Compare("double") == 0)) {
					nPStart += 2;
				}
				else {
					nPStart += 1;
				}

				listInputParam.push_back(cMethodParam);
			}
		}
		else if (strLine.Find(".end method") == 0) {
			// 函数结束
		}
		else if (strLine.Find(".locals") == 0) {
			// 本地变量定义， 用处不大，v XX的变量 我们要转变成 val 的变量
		}
		else if ((strLine.Find(".catch") == 0) || (strLine.Find(".catchall") == 0)) {
			// 不用理会这个段了，前面已经处理了
		}

		else if ((strLine.Find("sparse-switch") == 0) || (strLine.Find("packed-switch") == 0)) {		// 属于前段
																										// packed-switch 之后的 default 部分的结束部分，是要从case部分统计共同的结束位置，
																										// 默认switch之后，要新起一个段，这样可以决定这个段属于 default 部分还是 switch 外的部分, 统一使用 next 结束符号
																										// switch 指令		packed-switch 简单实用跳表，实际上就是一个数组，数值元素是地址值。跳表索引值 = base值(low) + n
																										// switch 指令		sparse-switch 实用键值对实现，一个key对应一个地址值
			int nDotFind = strLine.Find(":");
			CString strSwitchData = strLine.Right(strLine.GetLength() - nDotFind);
			strSwitchData.Trim();
			listSwitchData.push_back(strSwitchData);

			int nInstSn = AddInstructToOriginalList(strLine);
			instSegment.listInstSn.push_back(nInstSn);

			instSegment.listStrNext.clear();

			instSegment.listStrNext.push_back(CString("switch->") + strSwitchData);
			instSegment.listStrNext.push_back(CString("default->") + strLineName);
			instSegment.SegmentNextType = CJavaMethodSegment::nextSwitch;

			// 插入一个新段
			this->listMethodSegment.push_back(instSegment);
			instSegment.clear();
			instSegment.listSegName.push_back(strLineName);
		}
		else if (strLine.Find("return") == 0) {			// 属于前段
														// 返回指令
			int nInstSn = AddInstructToOriginalList(strLine);
			instSegment.listInstSn.push_back(nInstSn);

			// 插入一个新段
			instSegment.listStrNext.clear();
			instSegment.listStrNext.push_back(CString("return"));
			instSegment.SegmentNextType = CJavaMethodSegment::nextReturn;

			// 插入一个新段
			this->listMethodSegment.push_back(instSegment);
			instSegment.clear();
			instSegment.listSegName.push_back(strLineName);
		}
		else if (strLine.Find("throw") == 0) {			// 属于前段
			int nInstSn = AddInstructToOriginalList(strLine);
			instSegment.listInstSn.push_back(nInstSn);

			// 插入一个新段
			instSegment.listStrNext.clear();
			instSegment.listStrNext.push_back(CString("throw"));
			instSegment.SegmentNextType = CJavaMethodSegment::nextThrow;

			// 插入一个新段
			this->listMethodSegment.push_back(instSegment);
			instSegment.clear();
			instSegment.listSegName.push_back(strLineName);
		}
		else if (strLine.Find("if-") == 0) {				// 属于前段，处理else的方法是，从这个点开始，分析两边排异的部分

			std::vector<CString> listInstSeg = CJavaInstructItem::GetListInstructionSymbol(strLine);
			CString strLabel = listInstSeg[listInstSeg.size() - 1];
			strLabel.Trim();

			int nInstSn = AddInstructToOriginalList(strLine);
			instSegment.listInstSn.push_back(nInstSn);

			instSegment.listStrNext.clear();
			instSegment.listStrNext.push_back(CString("if-y->") + strLabel);		// 这个是需要跳转的行
			instSegment.listStrNext.push_back(CString("if-n->") + strLineName);		// 这个是默认不跳转的行
			instSegment.SegmentNextType = CJavaMethodSegment::nextIf;

			// 插入一个新段
			this->listMethodSegment.push_back(instSegment);
			instSegment.clear();
			instSegment.listSegName.push_back(strLineName);
		}
		else if (strLine.Find("try") == 0) {

			std::vector<CString> listI = CJavaInstructItem::GetListInstructionSymbol(strLine);

			// 插入指令
			int nInstSn = AddInstructToOriginalList(strLine);
			instSegment.listInstSn.push_back(nInstSn);

			instSegment.listStrNext.clear();

			instSegment.listStrNext.push_back(CString("try->") + listI[1]);
			instSegment.listStrNext.push_back(CString("default->") + listI[2]);
			instSegment.SegmentNextType = CJavaMethodSegment::nextTryCatch;

			// 插入一个新段
			this->listMethodSegment.push_back(instSegment);
			instSegment.clear();
			instSegment.listSegName.push_back(strLineName);
		}
		else if (strLine.Find("goto") == 0) {		// 属于前段

			std::vector<CString> listInstSeg = CJavaInstructItem::GetListInstructionSymbol(strLine);
			CString strLabel = listInstSeg[listInstSeg.size() - 1];
			strLabel.Trim();

			int nInstSn = AddInstructToOriginalList(strLine);
			instSegment.listInstSn.push_back(nInstSn);


			instSegment.listStrNext.clear();
			instSegment.listStrNext.push_back(CString("goto->") + strLabel);
			instSegment.SegmentNextType = CJavaMethodSegment::nextGoto;

			// 插入一个新段
			this->listMethodSegment.push_back(instSegment);
			instSegment.clear();
			instSegment.listSegName.push_back(strLineName);
		}
		else if (strLine.Find(".") == 0) {
			printf("未处理的\n");
		}
		else if (strLine.Find(":") == 0) {

			// 这个是找到行标号
			if (instSegment.listInstSn.size() > 0) {

				if (instSegment.listStrNext.size() == 0) {
					instSegment.listStrNext.push_back(CString("next->") + strLine);
					instSegment.SegmentNextType = CJavaMethodSegment::nextNormal;
				}

				// 插入一个新段
				this->listMethodSegment.push_back(instSegment);
				instSegment.clear();
			}
			instSegment.listSegName.push_back(strLine);
			//-------------------------------------------------------

			// 判断是否是 switch 架构
			bool bFindSwitch = false;
			if (listSwitchData.size() > 0) {
				std::vector<CString>::iterator switchit;
				for (switchit = listSwitchData.begin(); switchit != listSwitchData.end(); switchit++) {
					if (strLine.Compare(*switchit) == 0) {
						bFindSwitch = true;
						break;
					}
				}
			}

			// 下面的代码只是用来跳行用的
			if (bFindSwitch) {
				bool bPackedSwitch = false;
				bool bSparseSwitch = false;
				for (it++, nLineNumber++; it != listMethodInst.end(); it++, nLineNumber++) {
					strLine = (*it);
					if (strLine.Find(".packed-switch") == 0) {
						bPackedSwitch = true;
					}
					else if (strLine.Find(".end packed-switch") == 0) {
						if (!bPackedSwitch) {
							printf("格式有错误 %s %d\n", __FILE__, __LINE__);
						}
						break;
					}
					else if (strLine.Find(".sparse-switch") == 0) {
						bSparseSwitch = true;
					}
					else if (strLine.Find(".end sparse-switch") == 0) {
						if (!bSparseSwitch) {
							printf("格式有错误 %s %d\n", __FILE__, __LINE__);
						}
						break;
					}
				}
			}
		}
		else {
			int nInstSn = AddInstructToOriginalList(strLine);
			instSegment.listInstSn.push_back(nInstSn);
		}
	}
}