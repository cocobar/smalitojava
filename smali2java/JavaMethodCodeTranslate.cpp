#include "stdafx.h"
#include "JavaClass.h"
#include "JavaMethod.h"
#include "JavaInstructItem.h"

#include <algorithm>


static int TestCount = 0;

CString CJavaMethod::GetValueNameExpression(std::vector<CValueEqString> & listValueEqString, CString strValueName) {

	for (unsigned int i = 0; i < listValueEqString.size(); i++) {
		if (listValueEqString[i].strValueName.Compare(strValueName) == 0) {
			return listValueEqString[i].strExpression;
		}
	}
	return strValueName;
}

extern void ErrorNoInstructCode(CString strCode);


//SegmentListRetrodictGetRegInfo

// ���ϲ�������Ĵ��������ؼĴ��������ͣ��Ĵ�����full name
CString CJavaMethod::SegmentListRetrodictGetExpression(std::vector<int> listInstIndex, int nInsArryIndex, CString strRegName, int nInstIndex) {

	for (int i = nInsArryIndex; i > 0; i--) {
		std::vector<CJavaInstructItem>::iterator it = this->GetOriginalInstruct(listInstIndex[i - 1]);
		if (it == listOriginalInstruct.end()) {
			printf("�Ҳ������ %4d ��Ӧ��ָ��\n", listInstIndex[i]);
			goto fail;
		}

		// �ҵ�ƥ��ļĴ�����Ȼ����Ҫ���������Ҫ����������Ǳ��ʽ��
		if (it->regInfo.outReg.strRegName.Compare(strRegName) == 0) {
			return it->GetOutExpression();
		}
	}

	// ���û���ҵ�����ô����Ҫ�� listInputParam ����
	for (unsigned int i = 0; i < listInputParam.size(); i++) {
		if (listInputParam[i].strReg.Compare(strRegName) == 0) {
			return listInputParam[i].strValueName;
		}
	}
fail:

	printf("01û���ҵ�����Ĵ����Ķ���\n");
	{
		std::vector<CJavaInstructItem>::iterator it = this->GetOriginalInstruct(listInstIndex[nInsArryIndex]);
		printf("%4d %s\n", listInstIndex[nInsArryIndex], it->strInstString);
	}
	while (1) {};
	return CString("Unknow!");
}

bool strCheckIsNumber(CString strNumber) {
	if ((strNumber.Find("0x") == 0) || (strNumber.Find("0X") == 0)) {
		return true;
	}
	else {
		// 10 ����
		for (int i = 0; i < strNumber.GetLength(); i++) {
			char _c = strNumber[i];
			if ((_c < '0') || (_c > '9')) {
				return false;
			}
		}
	}
	return true;
}

unsigned int strGetNumberData(CString strNumber) {
	strNumber.Trim();
	if ((strNumber.Find("0x") == 0) || (strNumber.Find("0X") == 0)) {
		return CJavaMethod::GetDataFromHex(strNumber);
	}
	else {
		// 10 ����
		unsigned int nData = 0;

		for (int i = 0; i < strNumber.GetLength(); i++) {
			char _c = strNumber[i];

			nData *= 10;
			if ((_c >= '0') && (_c <= '9')) {
				nData += (_c - '0');
			}
		}

		return nData;
	}
}


// ��ʼ����ָ��
void CJavaMethod::SegmentCodeTranslate(int nLevel, std::vector<int> listInstIndex, std::vector<int> listNewInst) {
	unsigned int nStartIndex = listInstIndex.size();
	std::vector<int>::iterator itIndex = listInstIndex.insert(listInstIndex.end(), listNewInst.begin(), listNewInst.end());

	// ��ʼ���ϵ��·���
	for (unsigned int i = nStartIndex; itIndex < listInstIndex.end(); itIndex++, i++) {
		std::vector<CJavaInstructItem>::iterator it = this->GetOriginalInstruct((*itIndex));
		if (it == listOriginalInstruct.end()) {
			printf("�Ҳ������ %4d ��Ӧ��ָ��\n", (*itIndex));
			return;
		}

		// ��ָ���������
		CString strInstString = (*it).strInstString;
		int nInstIndex = (*itIndex);
		std::vector<CString> listI = CJavaInstructItem::GetListInstructionSymbol(strInstString);
		if (listI.size() > 0) {
			CString strCmd = listI[0];

			if (strCmd.Find("nop") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("move") == 0) {
				//  "move",					//move vx, vy	�ƶ�vy�����ݵ�vx�������Ĵ����������������256�Ĵ�����Χ���ڡ�
				//	"move/from16",			//move/from16 vx, vy	�ƶ�vy�����ݵ�vx��vy������64K�Ĵ�����Χ���ڣ���vx�����������256�Ĵ�����Χ���ڡ�
				//	"move/16",
				//	"move-wide",			//move-wide v4, v2   J-->J
				//	"move-wide/from16",		//move-wide/from16 vx, vy	�ƶ�һ��long/doubleֵ����vy��vx��vy������64K�Ĵ�����Χ���ڣ���vx�����������256�Ĵ�����Χ���ڡ�
				//	"move-wide/16",
				//	"move-object",			//move-object vx, vy	�ƶ��������ã���vy��vx��
				//	"move-object/from16",	//move-object/from16 vx, vy	�ƶ��������ã���vy��vx��vy���Դ���64K�Ĵ�����ַ��vx���Դ���256�Ĵ�����ַ��
				//	"move-object/16",

				//	"move-result",			//move-result vx	�ƶ���һ�η������õķ���ֵ��vx��
				//	"move-result-wide",		//move-result-wide vx	�ƶ���һ�η������õ�long/double�ͷ���ֵ��vx,vx+1��
				//	"move-result-object",	//move-result-object vx	�ƶ���һ�η������õĶ������÷���ֵ��vx��
				//	"move-exception",		//move-exception vx	�����������׳��쳣ʱ�ƶ��쳣�������õ�vx��

				CString strOutputR0;  CString strInputR0;
				if (strCmd.Find("move-result") == 0) {
					strOutputR0 = listI[1];		strInputR0 = CString("r0");
				}
				else if (strCmd.Find("move-exception") == 0) {
					strOutputR0 = listI[1];		strInputR0 = CString("e0");
				}
				else {
					strOutputR0 = listI[1];		strInputR0 = listI[2];
				}
				CString strExpression = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
				it->strCppExpression = strExpression;

				if (it->GetOutputCanBeShow()) {
					it->strCppCodeOutput.Format("%s = %s;", it->regInfo.outReg.strValName, it->strCppExpression);
					this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
					this->TouchTheVariableItem(it->regInfo.outReg.strValName);
					this->TouchTheVariableItem(it->strCppExpression);
				}
				else {
					it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);
				}

				printf("//%4d %s\n", it->nSerialNumber, it->strInstString);
				printf("%s\n", it->strCppCodeOutput);
			}
			else if (strCmd.Find("return") == 0) {
				//"return-void",			//return-void	���ؿ�ֵ��
				//"return",					//return vx	������vx�Ĵ�����ֵ��
				//"return-wide",			//return-wide vx	������vx,vx+1�Ĵ�����double/longֵ��
				//"return-object",			//return-object vx	������vx�Ĵ����Ķ������á�
				if (strCmd.Find("return-void") == 0) {
					// �޼Ĵ�������
					if (this->strReturnType.Compare("void")) {
						printf("����Ϊ void �����Ͳ�ƥ��\n");
						while (1) {};
					}

					// ������ܻ����������㷨
					it->bForceOutputCode = true;

					if (it->GetOutputCanBeShow()) {
						if (nLevel != 0) {
							it->strCppCodeOutput.Format("return;");
							this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
						}
					}
					else {
						it->strCppCodeOutput.Format("//return");
					}
				}
				else {
					// �мĴ���
					if (listI.size() == 2) {
						CString strInputR0 = listI[1];
						CString strExpression = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
						it->strCppExpression = strExpression;

						it->bForceOutputCode = true;

						if (it->GetOutputCanBeShow()) {
							it->strCppCodeOutput.Format("return %s;", it->strCppExpression);
							this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
							this->TouchTheVariableItem(it->strCppExpression);
						}
						else {
							it->strCppCodeOutput.Format("//return %s", it->strCppExpression);
						}
					}
					else {
						printf("%s ָ������д���\n", strInstString);
					}
				}

				printf("//%4d %s\n", it->nSerialNumber, it->strInstString);
				printf("%s\n", it->strCppCodeOutput);
			}
			else if (strCmd.Find("const") == 0) {
				if (listI.size() == 3) {
					CString strType;
					if (strCmd.Find("const/4") == 0) {						//const/4 vx, lit4					����4λ������vx��
						strType = CString("char");
						it->strCppExpression = listI[2];
					}
					else if (strCmd.Find("const/16") == 0) {				//const/16 vx, lit16				����16λ������vx��
						strType = CString("short");
						it->strCppExpression = listI[2];

						if (strCheckIsNumber(it->strCppExpression)) {
							unsigned int nNumber = strGetNumberData(it->strCppExpression);
							int iNumber = 0;
							memcpy(&iNumber, &nNumber, sizeof(unsigned int));
							it->strCppExpression.Format("%d", iNumber);
						}
					}
					else if (strCmd.Find("const/high16") == 0) {			//const/high16 v0, lit16			����16λ���������λ�Ĵ��������ڳ�ʼ��floatֵ��
						strType = CString("float");
						it->strCppExpression = listI[2];
					}
					else if (strCmd.Find("const-wide/16") == 0) {			//const-wide/16 vx, lit16			����int������vx, vx + 1�Ĵ�������չint�ͳ���Ϊlong������
						strType = CString("long");
						it->strCppExpression = listI[2];
					}
					else if (strCmd.Find("const-wide/32") == 0) {			//const - wide / 32 vx, lit32		����32λ������vx, vx + 1�Ĵ�������չint�ͳ�����long������
						strType = CString("long");
						it->strCppExpression = listI[2];
					}
					else if (strCmd.Find("const-wide") == 0) {				//const-wide vx, lit64				����64λ������vx, vx + 1�Ĵ���
						strType = CString("long");
						it->strCppExpression = listI[2];
					}
					else if (strCmd.Find("const-wide/high16") == 0) {		//const-wide/high16 vx, lit16		����16λ���������16λ��vx, vx + 1�Ĵ��������ڳ�ʼ��double ֵ��
						strType = CString("double");
						it->strCppExpression = listI[2];
					}
					else if (strCmd.Find("const-string") == 0) {			//const-string vx, �ַ���ID			�����ַ����������õ�vx��ͨ���ַ���ID���ַ�����
						strType = CJavaClass::GetTypeFromJava(CString("Ljava/lang/String;"));
						it->strCppExpression = listI[2];
					}
					else if (strCmd.Find("const-string/jumbo") == 0) {		//ͬ�ϣ��ִ�ID����ֵ���Գ���65535
						strType = CJavaClass::GetTypeFromJava(CString("Ljava/lang/String;"));
						it->strCppExpression = listI[2];
					}
					else if (strCmd.Find("const-class") == 0) {				//const-class vx, ����ID			�������������vx��ͨ������ID�����ͣ���Object.class����
						strType = CJavaClass::GetTypeFromJava(listI[2]);
						it->strCppExpression = CString("Unknow!");
					}
					else if (strCmd.Find("const") == 0) {					//const vx, lit32					����int �ͳ�����vx��
						strType = CString("int");
						it->strCppExpression = listI[2];

						if (strCheckIsNumber(it->strCppExpression)) {
							unsigned int nNumber = strGetNumberData(it->strCppExpression);
							int iNumber = 0;
							memcpy(&iNumber, &nNumber, sizeof(unsigned int));
							it->strCppExpression.Format("%d", iNumber);
						}
					}
					else {
						printf("%s ָ������д���\n", strInstString);
					}
				}
				else {
					printf("%s ָ������д���\n", strInstString);
				}

				// ���ڳ�������
				it->bConstValue = true;

				if (it->GetOutputCanBeShow()) {
					it->strCppCodeOutput.Format("%s = %s;", it->regInfo.outReg.strValName, it->strCppExpression);
					this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
					this->TouchTheVariableItem(it->regInfo.outReg.strValName);
					this->TouchTheVariableItem(it->strCppExpression);
				}
				else {
					it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);
				}

				printf("//%4d %s\n", it->nSerialNumber, it->strInstString);
				printf("%s\n", it->strCppCodeOutput);

			}
			else if (strCmd.Find("monitor-") == 0) {
				//printf("%s ָ��δ����\n", strInstString);

				CString strInputR0 = listI[1];
				CString strExpression = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
				it->strCppExpression = strExpression + CString("->monitor()");
				it->bForceOutputCode = true;

			}
			else if (strCmd.Find("check-cast") == 0) {					//check-cast vx, ����ID	���vx�Ĵ����еĶ��������Ƿ����ת��������ID��Ӧ���͵�ʵ�����粻��ת�����׳�ClassCastException�쳣���������ִ�С�
				if (listI.size() == 3) {

					CString strType = CJavaClass::GetTypeFromJava(listI[2]);
					CString strInputR0 = listI[1];
					CString strExpression = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
					it->strCppExpression = strExpression + CString("->checkType(\"") + strType + CString("\")");
					it->bForceOutputCode = true;
				}
				else {
					printf("%s ָ������д���\n", strInstString);
				}
			}
			else if (strCmd.Find("instance-of") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("array-length") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("new-") == 0) {

				//"new-instance",				//new-instance vx, ����ID	��������ID�������½�һ������ʵ���������½��Ķ�������ô���vx��
				//"new-array",					//new-array vx, vy, ����ID	��������ID�������½�һ�����飬vy��������ĳ��ȣ�vx������������á�
				if (strCmd.Find("new-instance") == 0)
				{
					if (listI.size() == 3) {
						CString strType = CJavaClass::GetTypeFromJava(listI[2]);
						it->strCppExpression = CString("new ") + strType;
					}
					else {
						printf("%s ָ������д���\n", strInstString);
					}

					// �����õļ���ǿ������Ϊ1
					it->regInfo.outReg.nOutregBeQuoteCount = 1;
				}
				else if (strCmd.Find("new-array") == 0) { // ʹ��vector��resize����
					if (listI.size() == 4) {
						CString strOutputR0 = listI[1];		CString strInputR0 = listI[2];
						CString strType = CJavaClass::GetTypeFromJava(listI[3]);
						it->strCppExpression = strType + CString(".resize(") + strInputR0 + CString(")");
					}
					else {
						printf("%s ָ������д���\n", strInstString);
					}
					it->bForceOutputCode = true;
				}

				it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);

				printf("//%4d %s\n", it->nSerialNumber, it->strInstString);
				printf("%s\n", it->strCppCodeOutput);
			}
			else if (strCmd.Find("fill") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("throw") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("goto") == 0) {
				//printf("%s ָ��δ����\n", strInstString);
				//"goto",			//goto Ŀ��	ͨ����ƫ����ע2��������ת��Ŀ�ꡣ
				//"goto/16",		//goto/16Ŀ��	ͨ��16λƫ����ע2��������ת��Ŀ�ꡣ
				//"goto/32",		//goto/32Ŀ��	ͨ��32λƫ����ע2��������ת��Ŀ�ꡣ
			}
			else if ((strCmd.Find("packed-switch") == 0) || (strCmd.Find("sparse-switch") == 0)) {
				//packed-switch vx, ������ƫ����	ʵ��һ��switch ��䣬case�����������ġ����ָ��ʹ��������vx���ڱ����ҵ�����case��ָ��ƫ����������������޷��ڱ����ҵ�vx��Ӧ������������ִ����һ��ָ���default case����
				if (listI.size() == 3) {
					CString strInputR0 = listI[1];
					CString strExpression = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
					it->strCppExpression = CString("switch (") + strExpression + CString(")");
				}
				else {
					printf("%s ָ������д���\n", strInstString);
				}
				it->bForceOutputCode = true;
			}
			else if (strCmd.Find("cmp") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("if-") == 0) {
				//printf("%s ָ��δ����\n", strInstString);
				//  "if-eq",	//if-eq vx,vy, Ŀ��	���vx == vyע2����ת��Ŀ�ꡣvx��vy��int��ֵ��	
				//	"if-ne",	//if-ne vx,vy, Ŀ��	���vx != vyע2����ת��Ŀ�ꡣvx��vy��int��ֵ��
				//	"if-lt",	//if-lt vx,vy, Ŀ��	���vx < vyע2����ת��Ŀ�ꡣvx��vy��int��ֵ��
				//	"if-ge",	//if-ge vx, vy, Ŀ��	���vx >= vyע2����ת��Ŀ�ꡣvx��vy��int��ֵ��
				//	"if-gt",	//if-gt vx,vy, Ŀ��	���vx > vyע2����ת��Ŀ�ꡣvx��vy��int��ֵ��
				//	"if-le",	//if-le vx,vy, Ŀ��	���vx <= vyע2����ת��Ŀ�ꡣvx��vy��int��ֵ��
				//	"if-eqz",	//if-eqz vx, Ŀ��	���vx == 0ע2����ת��Ŀ�ꡣvx��int��ֵ��
				//	"if-nez",	//if-nez vx, Ŀ��	���vx != 0ע2����ת��Ŀ��
				//	"if-ltz",	//if-ltz vx, Ŀ��	���vx < 0ע2����ת��Ŀ�ꡣ
				//	"if-gez",	//if-gez vx, Ŀ��	���vx >= 0ע2����ת��Ŀ�ꡣ
				//	"if-gtz",	//if-gtz vx, Ŀ��	���vx > 0ע2����ת��Ŀ�ꡣ
				//	"if-lez",	//if-lez vx, Ŀ��	���vx <= 0ע2����ת��Ŀ�ꡣ
				if (listI.size() == 4) {
					CString strInputR0 = listI[1];		CString strInputR1 = listI[2];

					CString strExpression0 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
					CString strExpression1 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR1, nInstIndex);

					if (strCmd.Compare("if-eq") == 0) {
						it->strCppExpression = CString("if (") + strExpression0 + CString("==") + strExpression1 + CString(")");
						it->strCppInverseExpression = CString("if (") + strExpression0 + CString("!=") + strExpression1 + CString(")");
					}
					else if (strCmd.Compare("if-ne") == 0) {
						it->strCppExpression = CString("if (") + strExpression0 + CString("!=") + strExpression1 + CString(")");
						it->strCppInverseExpression = CString("if (") + strExpression0 + CString("==") + strExpression1 + CString(")");
					}
					else if (strCmd.Compare("if-lt") == 0) {
						it->strCppExpression = CString("if (") + strExpression0 + CString("<") + strExpression1 + CString(")");
						it->strCppInverseExpression = CString("if (") + strExpression0 + CString(">=") + strExpression1 + CString(")");
					}
					else if (strCmd.Compare("if-ge") == 0) {
						it->strCppExpression = CString("if (") + strExpression0 + CString(">=") + strExpression1 + CString(")");
						it->strCppInverseExpression = CString("if (") + strExpression0 + CString("<") + strExpression1 + CString(")");
					}
					else if (strCmd.Compare("if-gt") == 0) {
						it->strCppExpression = CString("if (") + strExpression0 + CString(">") + strExpression1 + CString(")");
						it->strCppInverseExpression = CString("if (") + strExpression0 + CString("<=") + strExpression1 + CString(")");
					}
					else if (strCmd.Compare("if-le") == 0) {
						it->strCppExpression = CString("if (") + strExpression0 + CString("<=") + strExpression1 + CString(")");
						it->strCppInverseExpression = CString("if (") + strExpression0 + CString(">") + strExpression1 + CString(")");
					}
				}
				else if (listI.size() == 3) {
					CString strInputR0 = listI[1];
					CString strExpression0 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
					CString strExpression1 = CString("0");

					if (strCmd.Compare("if-eqz") == 0) {
						it->strCppExpression = CString("if (") + strExpression0 + CString("==") + strExpression1 + CString(")");
						it->strCppInverseExpression = CString("if (") + strExpression0 + CString(")");
					}
					else if (strCmd.Compare("if-nez") == 0) {
						it->strCppExpression = CString("if (") + strExpression0 + CString("!=") + strExpression1 + CString(")");
						it->strCppInverseExpression = CString("if (") + strExpression0 + CString("==") + strExpression1 + CString(")");
					}
					else if (strCmd.Compare("if-ltz") == 0) {
						it->strCppExpression = CString("if (") + strExpression0 + CString("<") + strExpression1 + CString(")");
						it->strCppInverseExpression = CString("if (") + strExpression0 + CString(">=") + strExpression1 + CString(")");
					}
					else if (strCmd.Compare("if-gez") == 0) {
						it->strCppExpression = CString("if (") + strExpression0 + CString(">=") + strExpression1 + CString(")");
						it->strCppInverseExpression = CString("if (") + strExpression0 + CString("<") + strExpression1 + CString(")");
					}
					else if (strCmd.Compare("if-gtz") == 0) {
						it->strCppExpression = CString("if (") + strExpression0 + CString(">") + strExpression1 + CString(")");
						it->strCppInverseExpression = CString("if (") + strExpression0 + CString("<=") + strExpression1 + CString(")");
					}
					else if (strCmd.Compare("if-lez") == 0) {
						it->strCppExpression = CString("if (") + strExpression0 + CString("<=") + strExpression1 + CString(")");
						it->strCppInverseExpression = CString("if (") + strExpression0 + CString(">") + strExpression1 + CString(")");
					}
				}
				else {
					printf("%s ָ������д��� %d %s\n", strInstString, __LINE__, __FILE__);
				}

				it->bForceOutputCode = true;
			}
			else if (strCmd.Find("aget") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("aput") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("iget") == 0) {
				//printf("%s ָ��δ����\n", strInstString);
				//"iget",				//iget vx, vy, �ֶ�ID	�����ֶ�ID��ȡʵ����int���ֶε�vx��vy�Ĵ������Ǹ�ʵ�������á�
				//"iget-wide",			//iget-wide vx, vy, �ֶ�ID	�����ֶ�ID��ȡʵ����double/long���ֶε�vx,vx+1ע1��vy�Ĵ������Ǹ�ʵ�������á�
				//"iget-object",		//iget-object vx, vy, �ֶ�ID	�����ֶ�ID��ȡһ��ʵ���Ķ��������ֶε�vx��vy�Ĵ������Ǹ�ʵ�������á�
				//"iget-boolean",		//iget-boolean vx, vy, �ֶ�ID	�����ֶ�ID��ȡʵ����boolean���ֶε�vx��vy�Ĵ������Ǹ�ʵ�������á�
				//"iget-byte",			//iget-byte vx, vy, �ֶ�ID	�����ֶ�ID��ȡʵ����byte���ֶε�vx��vy�Ĵ������Ǹ�ʵ�������á�
				//"iget-char",			//iget-char vx, vy, �ֶ�ID	�����ֶ�ID��ȡʵ����char���ֶε�vx��vy�Ĵ������Ǹ�ʵ�������á�
				//"iget-short",			//iget-short vx, vy, �ֶ�ID	�����ֶ�ID��ȡʵ����short���ֶε�vx��vy�Ĵ������Ǹ�ʵ�������á�
				if (listI.size() == 4) {
					CString strClassName = it->GetClassNameFromInstIput(listI[3]);
					CString strFieldName = it->GetFieldNameFromInstIput(listI[3]);
					CString strFieldType = it->GetFieldTypeFromInstIput(listI[3]);

					CString strInputR0 = listI[2];
					CString strExpression = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);

					if (strExpression == CString("this")) {
						it->strCppExpression = strFieldName;
					}
					else {
						it->strCppExpression = strExpression + CString(".") + strFieldName;
					}

				}
				else {
					printf("%s ָ������д���\n", strInstString);
				}

				if (it->GetOutputCanBeShow()) {
					it->strCppCodeOutput.Format("%s = %s;", it->regInfo.outReg.strValName, it->strCppExpression);
					this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
					this->TouchTheVariableItem(it->regInfo.outReg.strValName);
					this->TouchTheVariableItem(it->strCppExpression);
				}
				else {
					it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);
				}

				printf("//%4d %s\n", it->nSerialNumber, it->strInstString);
				printf("%s\n", it->strCppCodeOutput);

			}
			else if (strCmd.Find("iput") == 0) {
				//"iput",				// iput vx, vy, �ֶ�ID	�����ֶ�ID��vx�Ĵ�����ֵ����ʵ����int���ֶΣ�vy�Ĵ������Ǹ�ʵ�������á�
				//"iput-wide",			// iput-wide vx, vy, �ֶ�ID	�����ֶ�ID��vx,vx+1�Ĵ�����ֵ����ʵ����double/long���ֶΣ�vy�Ĵ������Ǹ�ʵ�������á�
				//"iput-object",		// iput-object vx, vy, �ֶ�ID	�����ֶ�ID��vx�Ĵ�����ֵ����ʵ���Ķ��������ֶΣ�vy�Ĵ������Ǹ�ʵ�������á�
				//"iput-boolean",		// iput-boolean vx, vy, �ֶ�ID	�����ֶ�ID��vx�Ĵ�����ֵ����ʵ����boolean���ֶΣ�vy�Ĵ������Ǹ�ʵ�������á�
				//"iput-byte",			// iput-byte vx, vy, �ֶ�ID	�����ֶ�ID��vx�Ĵ�����ֵ����ʵ����byte���ֶΣ�vy�Ĵ������Ǹ�ʵ�������á�
				//"iput-char",			// iput-char vx, vy, �ֶ�ID	�����ֶ�ID��vx�Ĵ�����ֵ����ʵ����char���ֶΣ�vy�Ĵ������Ǹ�ʵ�������á�
				//"iput-short",			// iput-short vx, vy, �ֶ�ID	�����ֶ�ID��vx�Ĵ�����ֵ����ʵ����short���ֶΣ�vy�Ĵ������Ǹ�ʵ�������á�
				//printf("%s ָ��δ����\n", strInstString);
				if (listI.size() == 4) {
					CString strClassName = it->GetClassNameFromInstIput(listI[3]);
					CString strFieldName = it->GetFieldNameFromInstIput(listI[3]);
					CString strFieldType = it->GetFieldTypeFromInstIput(listI[3]);

					CString strInputR0 = listI[1];		CString strInputR1 = listI[2];

					CString strExpression0 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
					CString strExpression1 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR1, nInstIndex);

					// �ж��ǲ���bool����
					if (strCmd.Compare("iput-boolean") == 0) {
						// �ж��ǲ������֣���������֣���ô��ת���� true, false;
						if (strCheckIsNumber(strExpression0)) {
							unsigned int nNumber = strGetNumberData(strExpression0);

							if (nNumber > 0) {
								strExpression0 = CString("true");
							}
							else {
								strExpression0 = CString("false");
							}
						}
					}
					else if (strCmd.Compare("iput") == 0) {
						if (strCheckIsNumber(strExpression0)) {
							unsigned int nNumber = strGetNumberData(strExpression0);
							int iNumber = 0;
							memcpy(&iNumber, &nNumber, sizeof(unsigned int));
							strExpression0.Format("%d", iNumber);
						}
					}

					if (strExpression1.Compare("this") == 0) {
						if (this->strMethodName.Compare(CString("<init>")) == 0) {
							it->strCppExpression = strFieldName + CString(" = ") + strExpression0;
						}
						else {
							it->strCppExpression = strFieldName + CString(" = ") + strExpression0;
						}
					}
					else {
						it->strCppExpression = strExpression1 + CString(".") + strFieldName + CString(" = ") + strExpression0;
					}

					it->bForceOutputCode = true;
				}
				else {
					printf("%s ָ������д���\n", strInstString);
				}

				if (it->GetOutputCanBeShow()) {
					it->strCppCodeOutput.Format("%s;", it->strCppExpression);
					this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
				}
				else {
					it->strCppCodeOutput.Format("//%s", it->strCppExpression);
				}

				printf("//%4d %s\n", it->nSerialNumber, it->strInstString);
				printf("%s\n", it->strCppCodeOutput);

			}
			else if (strCmd.Find("sget") == 0) {
				//  "sget",				//	sget vx, �ֶ�ID	�����ֶ�ID��ȡ��̬int���ֶε�vx��
				//	"sget-wide",		//  sget-wide vx, �ֶ�ID	�����ֶ�ID��ȡ��̬double/long���ֶε�vx,vx+1��
				//	"sget-object",		//  sget-object vx, �ֶ�ID	�����ֶ�ID��ȡ��̬���������ֶε�vx��
				//	"sget-boolean",		//  sget-boolean vx, �ֶ�ID	�����ֶ�ID��ȡ��̬boolean���ֶε�vx��
				//	"sget-byte",		//  sget-byte vx, �ֶ�ID	�����ֶ�ID��ȡ��̬byte���ֶε�vx��
				//	"sget-char",		//  sget-char vx, �ֶ�ID	�����ֶ�ID��ȡ��̬char���ֶε�vx��
				//	"sget-short",		//	sget-short vx, �ֶ�ID	�����ֶ�ID��ȡ��̬short���ֶε�vx��
				if (listI.size() == 3) {
					CString strClassName = it->GetClassNameFromInstIput(listI[2]);
					CString strFieldName = it->GetFieldNameFromInstIput(listI[2]);
					CString strFieldType = it->GetFieldTypeFromInstIput(listI[2]);

					it->strCppExpression = strClassName + CString("::") + strFieldName;
				}
				else {
					printf("%s ָ������д���\n", strInstString);
				}
			}
			else if (strCmd.Find("sput") == 0) {
				//  "sput",				// sput vx, �ֶ�ID	�����ֶ�ID��vx�Ĵ����е�ֵ��ֵ��int�;�̬�ֶΡ�
				//	"sput-wide",		// sput-wide vx, �ֶ�ID	�����ֶ�ID��vx,vx+1�Ĵ����е�ֵ��ֵ��double/long�;�̬�ֶΡ�
				//	"sput-object",		// sput-object vx, �ֶ�ID	�����ֶ�ID��vx�Ĵ����еĶ������ø�ֵ���������þ�̬�ֶΡ�
				//	"sput-boolean",		// sput-boolean vx, �ֶ�ID	�����ֶ�ID��vx�Ĵ����е�ֵ��ֵ��boolean�;�̬�ֶΡ�
				//	"sput-byte",		// sput-byte vx, �ֶ�ID	�����ֶ�ID��vx�Ĵ����е�ֵ��ֵ��byte�;�̬�ֶΡ�
				//	"sput-char",		// sput-char vx, �ֶ�ID	�����ֶ�ID��vx�Ĵ����е�ֵ��ֵ��char�;�̬�ֶΡ�
				//	"sput-short",		// sput-short vx, �ֶ�ID	�����ֶ�ID��vx�Ĵ����е�ֵ��ֵ��short�;�̬�ֶΡ�
				if (listI.size() == 3) {
					CString strClassName = it->GetClassNameFromInstIput(listI[2]);
					CString strFieldName = it->GetFieldNameFromInstIput(listI[2]);
					CString strFieldType = it->GetFieldTypeFromInstIput(listI[2]);

					CString strInputR0 = listI[1];
					CString strExpression = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
					it->strCppExpression = strClassName + CString("::") + strFieldName + CString("=") + strExpression;
					it->bForceOutputCode = true;
				}
				else {
					printf("%s ָ������д���\n", strInstString);
				}
			}
			else if (strCmd.Find("invoke-") == 0) {

				if (listI.size() == 3) {
					//"invoke-virtual/range",
					//"invoke-super/range",
					//"invoke-direct/range",
					//"invoke-static/range",
					//"invoke-interface/range",
					//"invoke-virtual",
					//"invoke-super",
					//"invoke-direct",
					//"invoke-static",
					//"invoke-interface",
					std::vector<CString> listReg;
					std::vector<CString>::iterator itReg;

					if (strInstString.Find("/range") > 0) {
						listReg = it->GetMethodCallRegListRange(listI[1]);
					}
					else {
						listReg = it->GetMethodCallRegList(listI[1]);
					}

					CString strClassName = it->GetClassNameFromInstInvoke(listI[2]);
					std::vector<CString> listParamType = it->GetSignatureFromInstInvoke(listI[2]);
					CString strReturnName = it->GetReturnTypeFromInstInvoke(listI[2]);
					CString strFunctionName = it->GetMethodNameFromInstInvoke(listI[2]);
					int nIndexParam = 0;

					CString strCppTotalExpression;

					if (strInstString.Find("invoke-static") == 0) {		// ����ȱһ������

						strCppTotalExpression = strClassName + CString("::") + strFunctionName + CString("(");

						for (itReg = listReg.begin(), nIndexParam = 0; itReg != listReg.end(); itReg++, nIndexParam++) {
							if (it->GetTotalSizeOfParam(listParamType) == listReg.size()) {
								CString strTypeName = listParamType[nIndexParam];

								CString strInputR0 = (*itReg);
								CString strExpression = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);

								if (strTypeName.Compare("double") == 0) {
									itReg++;
								}
								else if (strTypeName.Compare("long") == 0) {
									itReg++;
								}

								if ((itReg + 1) == listReg.end()) {
									strCppTotalExpression += strExpression;
								}
								else {
									strCppTotalExpression += (strExpression + CString(","));
								}
							}
							else {
								printf("%s ָ������д���\n", strInstString);
							}
						}
					}
					else {
						for (itReg = listReg.begin(), nIndexParam = 0; itReg != listReg.end(); itReg++, nIndexParam++) {
							if (it->GetTotalSizeOfParam(listParamType) == (listReg.size() - 1)) {
								if (nIndexParam == 0) {

									CString strInputR0 = (*itReg);
									CString strExpression = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);

									if ((strFunctionName.Compare("<init>") == 0) && (strExpression.Find("new ") == 0)) {
										strCppTotalExpression = strExpression + CString("(");

										// ���¼̳������������ǰ�Ĵ�����Ϊ����
										CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
										it->regInfo.outReg.strValName = varPair.strValName;
										it->regInfo.outReg.strRegName = varPair.strRegName;

										// ǿ�����ö�������
										//it->regInfo.outReg.nOutregBeQuoteCount = 100;
										it->regInfo.outReg.nOutregBeQuoteCount = varPair.nOutregBeQuoteCount - 1;
									}
									else {

										if (strExpression == CString("this")) {
											strCppTotalExpression = strFunctionName + CString("(");
										}
										else {
											strCppTotalExpression = strExpression + CString(".") + strFunctionName + CString("(");
										}
									}
								}
								else {
									CString strTypeName = listParamType[nIndexParam - 1];

									CString strInputR0 = (*itReg);
									CString strExpression = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);

									if (strTypeName.Compare("double") == 0) {
										itReg++;
									}
									else if (strTypeName.Compare("long") == 0) {
										itReg++;
									}
									if ((itReg + 1) == listReg.end()) {
										strCppTotalExpression += strExpression;
									}
									else {
										strCppTotalExpression += (strExpression + CString(","));
									}
								}
							}
							else {
								printf("%s ָ������д���\n", strInstString);
							}
						}
					}

					strCppTotalExpression += CString(")");

					it->strCppExpression = strCppTotalExpression;

					// Ĭ�ϵķ��ر���
					strReturnName.Trim();
					if (!strReturnName.IsEmpty()) {	// �з���ֵ
						if (strReturnName.Compare("void") != 0) {
							CString strOutputR0 = CString("r0");
							it->SetOutputRegInfo(strOutputR0, strReturnName, "", "");

							// û��������
							if (it->regInfo.outReg.listQuotePosition.size() == 0) {
								it->bForceOutputCode = true;
							}
						}
						else {
							it->bForceOutputCode = true;
						}
					}
					else {
						it->bForceOutputCode = true;
					}



					if (this->strMethodName.Compare("<init>") == 0) {
						if (strFunctionName.Compare("<init>") == 0) {
							// ��Ҫ�ж�Supper Class strClassName
							//if (this->strClassName.Compare())
							it->bForceOutputCode = false;
							it->bForceHideIt = true;
						}
					}

					if (it->GetOutputCanBeShow()) {
						if (it->regInfo.outReg.strValName.IsEmpty()) {
							it->strCppCodeOutput.Format("%s;", it->strCppExpression);
						}
						else {
							it->strCppCodeOutput.Format("%s = %s;", it->regInfo.outReg.strValName, it->strCppExpression);
							this->TouchTheVariableItem(it->regInfo.outReg.strValName);
							this->TouchTheVariableItem(it->strCppExpression);
						}
						this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
					}
					else {
						if (it->regInfo.outReg.strValName.IsEmpty()) {
							it->strCppCodeOutput.Format("//%s", it->strCppExpression);
						}
						else {
							it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);
						}
					}

					printf("//%4d %s\n", it->nSerialNumber, it->strInstString);
					printf("%s\n", it->strCppCodeOutput);

				}
				else {
					printf("%s ָ������д���\n", strInstString);
				}
			}
			else if (strCmd.Find("neg-") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("not-") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("int-to-") == 0) {
				//printf("%s ָ��δ����\n", strInstString);
				//"int-to-long"				//int-to-long vx, vy	ת��vy�Ĵ����е�int��ֵΪlong��ֵ����vx,vx+1��
				//"int-to-float",			//int-to-float vx, vy	ת��vy�Ĵ����е�int��ֵΪfloat��ֵ����vx��
				//"int-to-double",			//int-to-double vx, vy	ת��vy�Ĵ����е�int��ֵΪdouble��ֵ����vx,vx+1��
				//"int-to-byte",			//int-to-byte vx, vy	ת��vy�Ĵ����е�int��ֵΪbyte��ֵ����vx��
				//"int-to-char",			//int-to-char vx, vy	ת��vy�Ĵ����е�int��ֵΪchar��ֵ����vx��
				//"int-to-short",			//int-to-short vx, vy	ת��vy�Ĵ����е�int��ֵΪshort��ֵ����vx��

				CString strOutputR0;  CString strInputR0;

				strOutputR0 = listI[1];		strInputR0 = listI[2];

				CString strExpression1 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);

				if (listI.size() == 3) {
					if (listI[0].Compare("int-to-long") == 0) {
						it->strCppExpression.Format("((long)%s)", strExpression1);
					}
					else if (listI[0].Compare("int-to-float") == 0) {
						it->strCppExpression.Format("((float)%s)", strExpression1);
					}
					else if (listI[0].Compare("int-to-double") == 0) {
						it->strCppExpression.Format("((double)%s)", strExpression1);
					}
					else if (listI[0].Compare("int-to-byte") == 0) {
						it->strCppExpression.Format("((byte)%s)", strExpression1);
					}
					else if (listI[0].Compare("int-to-char") == 0) {
						it->strCppExpression.Format("((char)%s)", strExpression1);
					}
					else if (listI[0].Compare("int-to-short") == 0) {
						it->strCppExpression.Format("((short)%s)", strExpression1);
					}
					else {
						printf("%s ָ������д���\n", strInstString);
					}
				}
				else {
					printf("%s ָ������д���\n", strInstString);
				}

				if (it->GetOutputCanBeShow()) {
					it->strCppCodeOutput.Format("%s = %s;", it->regInfo.outReg.strValName, it->strCppExpression);
					this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
					this->TouchTheVariableItem(it->regInfo.outReg.strValName);
					this->TouchTheVariableItem(it->strCppExpression);
				}
				else {
					it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);
				}

				printf("//%4d %s\n", it->nSerialNumber, it->strInstString);
				printf("%s\n", it->strCppCodeOutput);

			}
			else if (strCmd.Find("long-to-") == 0) {
				//printf("%s ָ��δ����\n", strInstString);
				//  "long-to-int",			//long-to-int vx, vy	ת��vy,vy+1�Ĵ����е�long��ֵΪint��ֵ����vx��
				//	"long-to-float",		//long-to-float vx, vy	ת��vy,vy+1�Ĵ����е�long��ֵΪfloat��ֵ����vx��
				//	"long-to-double",		//long-to-double vx, vy	ת��vy,vy+1�Ĵ����е�long��ֵΪdouble��ֵ����vx,vx+1��

				CString strOutputR0;  CString strInputR0;

				strOutputR0 = listI[1];		strInputR0 = listI[2];

				CString strExpression1 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);

				if (listI.size() == 3) {
					if (listI[0].Compare("long-to-int") == 0) {
						it->strCppExpression.Format("((int)%s)", strExpression1);
					}
					else if (listI[0].Compare("long-to-float") == 0) {
						it->strCppExpression.Format("((float)%s)", strExpression1);
					}
					else if (listI[0].Compare("long-to-double") == 0) {
						it->strCppExpression.Format("((double)%s)", strExpression1);
					}
					else {
						printf("%s ָ������д���\n", strInstString);
					}
				}
				else {
					printf("%s ָ������д���\n", strInstString);
				}

				if (it->GetOutputCanBeShow()) {
					it->strCppCodeOutput.Format("%s = %s;", it->regInfo.outReg.strValName, it->strCppExpression);
					this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
					this->TouchTheVariableItem(it->regInfo.outReg.strValName);
					this->TouchTheVariableItem(it->strCppExpression);
				}
				else {
					it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);
				}

				printf("//%4d %s\n", it->nSerialNumber, it->strInstString);
				printf("%s\n", it->strCppCodeOutput);

			}
			else if (strCmd.Find("float-to-") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("double-to-") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("add-") == 0) {
				//printf("%s ָ��δ����\n", strInstString);
				//  "add-int",				//add-int vx, vy, vz	����vy + vz�����������vx��
				//  "add-long",				//add-long vx, vy, vz	����vy,vy+1 + vz,vz+1�����������vx,vx+1ע1��
				//	"add-float",			//add-long vx, vy, vz	����vy,vy+1 + vz,vz+1�����������vx,vx+1ע1��
				//	"add-double",			//add-double vx, vy, vz	����vy,vy+1 + vz,vz+1�����������vx,vx+1ע1��
				//	"add-int/2addr",		//add-int/2addr vx, vy	����vx + vy�����������vx��
				//	"add-long/2addr",		//add-long/2addr vx, vy	����vx,vx+1 + vy,vy+1�����������vx,vx+1ע1��
				//	"add-float/2addr",		//add-float/2addr vx, vy	����vx + vy�����������vx��
				//	"add-double/2addr",		//add-double/2addr vx, vy	����vx,vx+1 + vy,vy+1�����������vx,vx+1ע1��
				//	"add-int/lit16",		//add-int/lit16 vx, vy, lit16	����vy + lit16�����������vx��
				//	"add-int/lit8",			//add-int/lit8 vx, vy, lit8	����vy + lit8�����������vx
				if ((listI.size() == 3)) {

					CString strOutputR0;  CString strInputR0;

					strOutputR0 = listI[1];		strInputR0 = listI[2];

					CString strExpression1 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
					CString strExpression2 = SegmentListRetrodictGetExpression(listInstIndex, i, strOutputR0, nInstIndex);

					it->strCppExpression.Format("(%s + %s)", strExpression2, strExpression1);

					if (it->GetOutputCanBeShow()) {
						it->strCppCodeOutput.Format("%s = %s;", it->regInfo.outReg.strValName, it->strCppExpression);
						this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
						this->TouchTheVariableItem(it->regInfo.outReg.strValName);
						this->TouchTheVariableItem(it->strCppExpression);
					}
					else {
						it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);
					}
#if 0
					if (listI[0].Compare("add-int/2addr") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("add-long/2addr") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("add-float/2addr") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("add-double/2addr") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else {
						printf("%s ָ������д���\n", strInstString);
					}
#endif
				}
				else if (listI.size() == 4) {

					// add-int/lit16 vx, vy, lit16	����vy + lit16�����������vx��
					// "add-float",			//add-long vx, vy, vz	����vy,vy+1 + vz,vz+1�����������vx,vx+1ע1��

					CString strOutputR0;  CString strInputR0; CString strInputR1;
					strOutputR0 = listI[1];		strInputR0 = listI[2];	strInputR1 = listI[3];

					CString strExpression1 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
					CString strExpression2;
					if ((listI[0].Compare("add-int/lit16") == 0) || (listI[0].Compare("add-int/lit8") == 0)) {
						strExpression2 = strInputR0;
					}
					else {
						strExpression2 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR1, nInstIndex);
					}

					it->strCppExpression.Format("(%s + %s)", strExpression2, strExpression1);

					if (it->GetOutputCanBeShow()) {
						it->strCppCodeOutput.Format("%s = %s;", it->regInfo.outReg.strValName, it->strCppExpression);
						this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
						this->TouchTheVariableItem(it->regInfo.outReg.strValName);
						this->TouchTheVariableItem(it->strCppExpression);
					}
					else {
						it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);
					}
#if 0
					if (listI[0].Compare("add-int") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("add-long") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("add-float") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("add-int/lit16") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("add-int/lit8") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else {
						printf("%s ָ������д���\n", strInstString);
					}
#endif
				}
				else {
					printf("%s ָ������д���\n", strInstString);
				}
			}
			else if (strCmd.Find("sub-") == 0) {
				//printf("%s ָ��δ����\n", strInstString);
				//  "sub-int",				//sub-int vx, vy, vz	����vy - vz�����������vx��
				//	"sub-long",				//sub-long vx, vy, vz	����vy,vy+1 - vz,vz+1�����������vx,vx+1ע1��
				//	"sub-float",			//sub-float vx, vy, vz	����vy - vz�����������vx��
				//	"sub-double",			//sub-double vx, vy, vz	����vy,vy+1 - vz,vz+1�����������vx,vx+1ע1��
				//	"sub-int/2addr",		//sub-int/2addr vx, vy	����vx - vy�����������vx��
				//	"sub-long/2addr",		//sub-long/2addr vx, vy	����vx,vx+1 - vy,vy+1�����������vx,vx+1ע1��
				//	"sub-float/2addr",		//sub-float/2addr vx, vy	����vx - vy�����������vx��
				//	"sub-double/2addr",		//sub-double/2addr vx, vy	����vx,vx+1 - vy,vy+1�����������vx,vx+1ע1��
				if ((listI.size() == 3)) {

					CString strOutputR0;  CString strInputR0;

					strOutputR0 = listI[1];		strInputR0 = listI[2];

					CString strExpression1 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
					CString strExpression2 = SegmentListRetrodictGetExpression(listInstIndex, i, strOutputR0, nInstIndex);

					it->strCppExpression.Format("(%s - %s)", strExpression2, strExpression1);

					if (it->GetOutputCanBeShow()) {
						it->strCppCodeOutput.Format("%s = %s;", it->regInfo.outReg.strValName, it->strCppExpression);
						this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
						this->TouchTheVariableItem(it->regInfo.outReg.strValName);
						this->TouchTheVariableItem(it->strCppExpression);
					}
					else {
						it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);
					}

#if 0
					if (listI[0].Compare("sub-int/2addr") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("sub-long/2addr") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("sub-float/2addr") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("sub-double/2addr") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else {
						printf("%s ָ������д���\n", strInstString);
					}
#endif
				}
				else if (listI.size() == 4) {

					CString strOutputR0;  CString strInputR0;

					strOutputR0 = listI[2];		strInputR0 = listI[3];

					CString strExpression1 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
					CString strExpression2 = SegmentListRetrodictGetExpression(listInstIndex, i, strOutputR0, nInstIndex);

					it->strCppExpression.Format("(%s - %s)", strExpression2, strExpression1);

					if (it->GetOutputCanBeShow()) {
						it->strCppCodeOutput.Format("%s = %s;", it->regInfo.outReg.strValName, it->strCppExpression);
						this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
						this->TouchTheVariableItem(it->regInfo.outReg.strValName);
						this->TouchTheVariableItem(it->strCppExpression);
					}
					else {
						it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);
					}
				}
				else {
					printf("%s ָ������д���\n", strInstString);
				}

			}
			else if (strCmd.Find("mul-") == 0) {
				//printf("%s ָ��δ����\n", strInstString);
				//  "mul-int",				// mul-int vx, vy, vz	����vy * vz�����������vx��
				//	"mul-long",				// mul-long vx, vy, vz	����vy,vy+1 * vz,vz+1�����������vx,vx+1ע1��
				//	"mul-float",			// mul-float vx, vy, vz	����vy * vz�����������vx��
				//	"mul-double",			// mul-double vx, vy, vz	����vy,vy+1 * vz,vz+1�����������vx,vx+1ע1��
				//	"mul-int/2addr"			// mul-int/2addr vx, vy	����vx * vy�����������vx��
				//	"mul-long/2addr",		// mul-long/2addr vx, vy	����vx,vx+1 * vy,vy+1�����������vx,vx+1ע1��
				//	"mul-float/2addr",		// mul-float/2addr vx, vy	����vx * vy�����������vx��
				//	"mul-double/2addr",		// mul-double/2addr vx, vy	����vx,vx+1 * vy,vy+1�����������vx,vx+1ע1��
				//	"mul-int/lit16",		// mul-int/lit16 vx, vy, lit16	����vy * lit16�����������vx��
				//	"mul-int/lit8",			// mul-int/lit8 vx, vy, lit8	����vy * lit8�����������vx��
				if ((listI.size() == 3)) {

					CString strOutputR0;  CString strInputR0;

					strOutputR0 = listI[1];		strInputR0 = listI[2];

					CString strExpression1 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
					CString strExpression2 = SegmentListRetrodictGetExpression(listInstIndex, i, strOutputR0, nInstIndex);

					it->strCppExpression.Format("(%s * %s)", strExpression2, strExpression2, strExpression1);

					if (it->GetOutputCanBeShow()) {
						it->strCppCodeOutput.Format("%s = %s;", it->regInfo.outReg.strValName, it->strCppExpression);
						this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
						this->TouchTheVariableItem(it->regInfo.outReg.strValName);
						this->TouchTheVariableItem(it->strCppExpression);
					}
					else {
						it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);
					}

					printf("//%4d %s\n", it->nSerialNumber, it->strInstString);
					printf("%s\n", it->strCppCodeOutput);
				}
				else if (listI.size() == 4) {
					if (listI[0].Compare("mul-int") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("mul-long") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("mul-float") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("mul-int/lit16") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("mul-int/lit8") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else {
						printf("%s ָ������д���\n", strInstString);
					}
				}
				else {
					printf("%s ָ������д���\n", strInstString);
				}
			}
			else if (strCmd.Find("div-") == 0) {
				//printf("%s ָ��δ����\n", strInstString);
				//  "div-int",					//div-int vx, vy, vz	����vy / vz�����������vx��
				//	"div-long",					//div-long vx, vy, vz	����vy,vy+1 / vz,vz+1�����������vx,vx+1ע1��
				//	"div-float",				//div-float vx, vy, vz	����vy / vz�����������vx��
				//	"div-double",				//div-double vx, vy, vz	����vy,vy+1 / vz,vz+1�����������vx,vx+1ע1��
				//	"div-int/2addr",			//div-int/2addr vx, vy	����vx / vy�����������vx��
				//	"div-long/2addr",			//div-long/2addr vx, vy	����vx,vx+1 / vy,vy+1�����������vx,vx+1ע1��
				//	"div-float/2addr",			//div-float/2addr vx, vy	����vx / vy�����������vx��
				//	"div-double/2addr",			//div-double/2addr vx, vy	����vx,vx+1 / vy,vy+1�����������vx,vx+1ע1��
				//	"div-int/lit16",			//div-int/lit16 vx, vy, lit16	����vy / lit16�����������vx��
				//	"div-int/lit8",				//div-int/lit8 vx, vy, lit8	����vy / lit8�����������vx��
				if ((listI.size() == 3)) {

					CString strOutputR0;  CString strInputR0;

					strOutputR0 = listI[1];		strInputR0 = listI[2];

					CString strExpression1 = SegmentListRetrodictGetExpression(listInstIndex, i, strInputR0, nInstIndex);
					CString strExpression2 = SegmentListRetrodictGetExpression(listInstIndex, i, strOutputR0, nInstIndex);

					it->strCppExpression.Format("(%s / %s)", strExpression2, strExpression2, strExpression1);

					if (it->GetOutputCanBeShow()) {
						it->strCppCodeOutput.Format("%s = %s;", it->regInfo.outReg.strValName, it->strCppExpression);
						this->InsertCppStringCode(nLevel + 1, it->strCppCodeOutput);
						this->TouchTheVariableItem(it->regInfo.outReg.strValName);
						this->TouchTheVariableItem(it->strCppExpression);
					}
					else {
						it->strCppCodeOutput.Format("//%s = %s", it->regInfo.outReg.strValName, it->strCppExpression);
					}
#if 0
					if (listI[0].Compare("div-int/2addr") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("div-long/2addr") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("div-float/2addr") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("div-double/2addr") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else {
						printf("%s ָ������д���\n", strInstString);
					}
#endif
				}
				else if (listI.size() == 4) {
					if (listI[0].Compare("div-int") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("div-long") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("div-float") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("div-int/lit16") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("div-int/lit8") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else {
						printf("%s ָ������д���\n", strInstString);
					}
				}
				else {
					printf("%s ָ������д���\n", strInstString);
				}
			}
			else if (strCmd.Find("rem-") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("and-") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("or-") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("xor-") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("shl-") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("shr-") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("ushr-") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("rsub-int") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("+") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("^breakpoint") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("^throw-verification-error") == 0) {
				ErrorNoInstructCode(strInstString);
			}
			else if (strCmd.Find("try") == 0) {
				// ����ָ���Ҫ����
				it->strCppExpression = CString("try");
				it->bForceOutputCode = true;
			}
			else if (strCmd.Find("catch") == 0) {
				//e�Ĵ���
				it->strCppExpression = CString("catch");
				it->bForceOutputCode = true;
			}
			else {
				printf("%s ָ������д���\n", strInstString);
				while (1) {};
			}
		}
		else {
			printf("%s ָ������д���\n", strInstString);
		}
	}

	return;
}