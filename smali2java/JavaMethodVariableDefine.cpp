#include "stdafx.h"
#include "JavaClass.h"
#include "JavaMethod.h"
#include "JavaInstructItem.h"

#include <algorithm>

// ���ϲ�������Ĵ��������ؼĴ��������ͣ��Ĵ�����full name
CRegVariablePair CJavaMethod::SegmentListRetrodictGetRegInfo(std::vector<int> listInstIndex, int nInsArryIndex, CString strRegName, int nInstIndex) {

	CRegVariablePair varPair;

	for (int i = nInsArryIndex; i > 0; i--) {
		std::vector<CJavaInstructItem>::iterator it = this->GetOriginalInstruct(listInstIndex[i - 1]);
		if (it == listOriginalInstruct.end()) {
			printf("�Ҳ������ %4d ��Ӧ��ָ��\n", listInstIndex[i]);
			goto fail;
		}

		if (it->regInfo.outReg.strRegName.Compare(strRegName) == 0) {

			CString strPos;
			strPos.Format("%d-%s", nInstIndex, strRegName);
			it->regInfo.outReg.listQuotePosition.push_back(strPos);

			// ȥ��
			sort(it->regInfo.outReg.listQuotePosition.begin(), it->regInfo.outReg.listQuotePosition.end());
			std::vector<CString>::iterator iter = std::unique(it->regInfo.outReg.listQuotePosition.begin(), it->regInfo.outReg.listQuotePosition.end());
			it->regInfo.outReg.listQuotePosition.erase(iter, it->regInfo.outReg.listQuotePosition.end());

			return it->regInfo.outReg;
		}
	}

	// ���û���ҵ�����ô����Ҫ�� listInputParam ����
	for (unsigned int i = 0; i < listInputParam.size(); i++) {
		if (listInputParam[i].strReg.Compare(strRegName) == 0) {
			varPair.strRegName = listInputParam[i].strReg;
			varPair.strStrongType = listInputParam[i].strTypeName;
			varPair.strValName = listInputParam[i].strValueName;
			varPair.belongInstIndex = -1;
			varPair.listQuotedInstIndex.clear();
			return varPair;
		}
	}
fail:

	printf("02û���ҵ�����Ĵ����Ķ���\n");
	{
		std::vector<CJavaInstructItem>::iterator it = this->GetOriginalInstruct(listInstIndex[nInsArryIndex]);
		printf("%4d %s\n", listInstIndex[nInsArryIndex], it->strInstString);
	}
	while (1) {};
	return varPair;
}

void ErrorNoInstructCode(CString strCode) {
	printf("δ�����ָ�� %s\n", strCode);
	while (1) {};
}

bool CJavaMethod::SegmentListInstSpread(std::vector<int> listSegIndex)
{
	// ��ȡָ���嵥
	std::vector<int> listInstIndex;
	listInstIndex.clear();
	for (unsigned int i = 0; i < listSegIndex.size(); i++) {
		for (unsigned int j = 0; j < this->listMethodSegment[listSegIndex[i]].listInstSn.size(); j++) {
			listInstIndex.push_back(this->listMethodSegment[listSegIndex[i]].listInstSn[j]);
		}
	}
	for (unsigned int i = 0; i < listInstIndex.size(); i++) {
		std::vector<CJavaInstructItem>::iterator it = this->GetOriginalInstruct(listInstIndex[i]);
		if (it == listOriginalInstruct.end()) {
			printf("�Ҳ������ %4d ��Ӧ��ָ��\n", listInstIndex[i]);
			return false;
		}

		// ��ָ���������
		CString strInstString = (*it).strInstString;
		int nInstIndex = listInstIndex[i];

		if (it->regInfo.listIreg.size() > 0) {
			for (unsigned int j = 0; j < it->regInfo.listIreg.size(); j++) {
				CString strInputR0 = it->regInfo.listIreg[j].strRegName;
				CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
				if (varPair.strValName.IsEmpty()) {
					printf("���ش���������������ֻ�û�ж���\n");
					while (1) {};
				}
				else {
					if (it->regInfo.listIreg[j].strValName.IsEmpty()) {
						it->regInfo.listIreg[j].strValName = varPair.strValName;
					}
					else {
						if (it->regInfo.listIreg[j].strValName.Compare(varPair.strValName) != 0) {

							printf("�Ĵ������� %s\n", it->regInfo.listIreg[j].strRegName);
							printf("�Ѿ����� %s\n", it->regInfo.listIreg[j].strValName);
							printf("��Ҫ���� %s\n", varPair.strValName);

							printf("%4d %s\n", it->nSerialNumber, it->strInstString);
							printf("���ش���һ�������ж������\n");
							while (1) {};
						}
					}
				}
			}
		}
	}

	return true;
}

// ������������ָ��
bool CJavaMethod::SegmentListInstRebuild(std::vector<int> listSegIndex) {

	// ��ȡָ���嵥
	std::vector<int> listInstIndex;
	listInstIndex.clear();
	for (unsigned int i = 0; i < listSegIndex.size(); i++) {
		for (unsigned int j = 0; j < this->listMethodSegment[listSegIndex[i]].listInstSn.size(); j++) {
			listInstIndex.push_back(this->listMethodSegment[listSegIndex[i]].listInstSn[j]);
		}
	}

	// ��ʼ���ϵ��·���
	for (unsigned int i = 0; i < listInstIndex.size(); i++) {
		std::vector<CJavaInstructItem>::iterator it = this->GetOriginalInstruct(listInstIndex[i]);
		if (it == listOriginalInstruct.end()) {
			printf("�Ҳ������ %4d ��Ӧ��ָ��\n", listInstIndex[i]);
			return false;
		}

		// ��ָ���������
		CString strInstString = (*it).strInstString;
		int nInstIndex = listInstIndex[i];
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
				CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
				it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
				it->SetOutputRegInfo(strOutputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName);
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
				}
				else {
					// �мĴ���
					if (listI.size() == 2) {
						CString strInputR0 = listI[1];
						CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
						it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);

						// ����һ��ǿ����
						it->CheckInputRegStrongType(strInputR0, this->strReturnType);
					}
					else {
						printf("%s ָ������д���\n", strInstString);
					}
				}
			}
			else if (strCmd.Find("const") == 0) {
				if (listI.size() == 3) {
					CString strType;
					if (strCmd.Find("const/4") == 0) {						//const/4 vx, lit4					����4λ������vx��
						strType = CString("char");
					}
					else if (strCmd.Find("const/16") == 0) {				//const/16 vx, lit16				����16λ������vx��
						strType = CString("short");
					}
					else if (strCmd.Find("const/high16") == 0) {			//const/high16 v0, lit16			����16λ���������λ�Ĵ��������ڳ�ʼ��floatֵ��
						strType = CString("float");
					}
					else if (strCmd.Find("const-wide/16") == 0) {			//const-wide/16 vx, lit16			����int������vx, vx + 1�Ĵ�������չint�ͳ���Ϊlong������
						strType = CString("long");
					}
					else if (strCmd.Find("const-wide/32") == 0) {			//const - wide / 32 vx, lit32		����32λ������vx, vx + 1�Ĵ�������չint�ͳ�����long������
						strType = CString("long");
					}
					else if (strCmd.Find("const-wide") == 0) {				//const-wide vx, lit64				����64λ������vx, vx + 1�Ĵ���
						strType = CString("long");
					}
					else if (strCmd.Find("const-wide/high16") == 0) {		//const-wide/high16 vx, lit16		����16λ���������16λ��vx, vx + 1�Ĵ��������ڳ�ʼ��double ֵ��
						strType = CString("double");
					}
					else if (strCmd.Find("const-string") == 0) {			//const-string vx, �ַ���ID			�����ַ����������õ�vx��ͨ���ַ���ID���ַ�����
						strType = CJavaClass::GetTypeFromJava(CString("Ljava/lang/String;"));
					}
					else if (strCmd.Find("const-string/jumbo") == 0) {		//ͬ�ϣ��ִ�ID����ֵ���Գ���65535
						strType = CJavaClass::GetTypeFromJava(CString("Ljava/lang/String;"));
					}
					else if (strCmd.Find("const-class") == 0) {				//const-class vx, ����ID			�������������vx��ͨ������ID�����ͣ���Object.class����
						strType = CJavaClass::GetTypeFromJava(listI[2]);
					}
					else if (strCmd.Find("const") == 0) {					//const vx, lit32					����int �ͳ�����vx��
						strType = CString("int");
					}
					else {
						printf("%s ָ������д���\n", strInstString);
					}

					CString strOutputR0 = listI[1];
					it->SetOutputRegInfo(strOutputR0, "", strType, "");
				}
				else {
					printf("%s ָ������д���\n", strInstString);
				}
			}
			else if (strCmd.Find("monitor-") == 0) {
				//printf("%s ָ��δ����\n", strInstString);
				CString strInputR0 = listI[1];
				CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
				it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
			}
			else if (strCmd.Find("check-cast") == 0) {					//check-cast vx, ����ID	���vx�Ĵ����еĶ��������Ƿ����ת��������ID��Ӧ���͵�ʵ�����粻��ת�����׳�ClassCastException�쳣���������ִ�С�
				if (listI.size() == 3) {
					CString strInputR0 = listI[1];
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);

					CString strType = CJavaClass::GetTypeFromJava(listI[2]);
					it->CheckInputRegStrongType(strInputR0, strType);
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
						CString strOutputR0 = listI[1];
						CString strType = CJavaClass::GetTypeFromJava(listI[2]);
						it->SetOutputRegInfo(strOutputR0, strType, "", "");
					}
					else {
						printf("%s ָ������д���\n", strInstString);
					}
				}
				else if (strCmd.Find("new-array") == 0) {
					if (listI.size() == 4) {
						CString strOutputR0 = listI[1];		CString strInputR0 = listI[2];
						CString strType = CJavaClass::GetTypeFromJava(listI[3]);

						CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
						it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
						it->SetOutputRegInfo(strOutputR0, strType, "", "");
					}
					else {
						printf("%s ָ������д���\n", strInstString);
					}
				}
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
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
				}
				else {
					printf("%s ָ������д���\n", strInstString);
				}
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
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);

					varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR1, nInstIndex);
					it->SetInputRegInfo(strInputR1, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
				}
				else if (listI.size() == 3) {
					CString strInputR0 = listI[1];
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
				}
				else {
					printf("%s ָ������д��� %d %s\n", strInstString, __LINE__, __FILE__);
				}
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

					CString strOutputR0 = listI[1];		CString strInputR0 = listI[2];
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					it->SetOutputRegInfo(strOutputR0, strFieldType, "", "");
					it->CheckInputRegStrongType(strInputR0, strClassName);
				}
				else {
					printf("%s ָ������д���\n", strInstString);
				}
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
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);

					varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR1, nInstIndex);
					it->SetInputRegInfo(strInputR1, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);

					it->CheckInputRegStrongType(strInputR0, strFieldType);
					it->CheckInputRegStrongType(strInputR1, strClassName);
				}
				else {
					printf("%s ָ������д���\n", strInstString);
				}
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

					CString strOutputR0 = listI[1];
					it->SetOutputRegInfo(strOutputR0, strFieldType, "", "");
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
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					it->CheckInputRegStrongType(strInputR0, strFieldType);
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
					int nIndexParam = 0;

					if (strInstString.Find("invoke-static") == 0) {		// ����ȱһ������
						for (itReg = listReg.begin(), nIndexParam = 0; itReg != listReg.end(); itReg++, nIndexParam++) {
							if (it->GetTotalSizeOfParam(listParamType) == listReg.size()) {
								CString strTypeName = listParamType[nIndexParam];

								CString strInputR0 = (*itReg);
								CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
								it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);

								it->CheckInputRegStrongType(strInputR0, strTypeName);


								if (strTypeName.Compare("double") == 0) {
									itReg++;
								}
								else if (strTypeName.Compare("long") == 0) {
									itReg++;
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
									CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
									it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
								}
								else {
									CString strTypeName = listParamType[nIndexParam - 1];

									CString strInputR0 = (*itReg);
									CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
									it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
									it->CheckInputRegStrongType(strInputR0, strTypeName);


									if (strTypeName.Compare("double") == 0) {
										itReg++;
									}
									else if (strTypeName.Compare("long") == 0) {
										itReg++;
									}
								}
							}
							else {
								printf("%s ָ������д���\n", strInstString);
							}
						}
					}

					// Ĭ�ϵķ��ر���
					strReturnName.Trim();
					if (!strReturnName.IsEmpty()) {
						if (strReturnName.Compare("void") != 0) {
							CString strOutputR0 = CString("r0");
							it->SetOutputRegInfo(strOutputR0, strReturnName, "", "");
						}
					}
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
				if (listI.size() == 3) {
					if (listI[0].Compare("int-to-long") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("int-to-float") == 0) {
						CString strInputR0 = listI[1];
						CString strOutputR0 = listI[1];
						CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
						it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);

						it->SetOutputRegInfo(strOutputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName);

						it->CheckInputRegStrongType(strInputR0, CString("int"));
					}
					else if (listI[0].Compare("int-to-double") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("int-to-byte") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("int-to-char") == 0) {
						ErrorNoInstructCode(strInstString);
					}
					else if (listI[0].Compare("int-to-short") == 0) {
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
			else if (strCmd.Find("long-to-") == 0) {
				//printf("%s ָ��δ����\n", strInstString);
				//  "long-to-int",			//long-to-int vx, vy	ת��vy,vy+1�Ĵ����е�long��ֵΪint��ֵ����vx��
				//	"long-to-float",		//long-to-float vx, vy	ת��vy,vy+1�Ĵ����е�long��ֵΪfloat��ֵ����vx��
				//	"long-to-double",		//long-to-double vx, vy	ת��vy,vy+1�Ĵ����е�long��ֵΪdouble��ֵ����vx,vx+1��
				if (listI.size() == 3) {
					CString strInputR0 = listI[1];
					CString strOutputR0 = listI[1];
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					it->SetOutputRegInfo(strOutputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName);
					it->CheckInputRegStrongType(strInputR0, CString("long"));
				}
				else {
					printf("%s ָ������д���\n", strInstString);
				}
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

					CString strInputR0 = listI[1];		CString strInputR1 = listI[2];
					CString strOutputR0 = listI[1];
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR1, nInstIndex);
					it->SetInputRegInfo(strInputR1, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					it->SetOutputRegInfo(strOutputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName);

					if (listI[0].Compare("add-int/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("int"));
						it->CheckInputRegStrongType(strInputR1, CString("int"));
					}
					else if (listI[0].Compare("add-long/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("long"));
						it->CheckInputRegStrongType(strInputR1, CString("long"));
					}
					else if (listI[0].Compare("add-float/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("float"));
						it->CheckInputRegStrongType(strInputR1, CString("float"));
					}
					else if (listI[0].Compare("add-double/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("double"));
						it->CheckInputRegStrongType(strInputR1, CString("double"));

					}
					else {
						printf("%s ָ������д���\n", strInstString);
					}
				}
				else if (listI.size() == 4) {

					CString strInputR0 = listI[2];		CString strInputR1 = listI[3];
					CString strOutputR0 = listI[1];
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					it->SetOutputRegInfo(strOutputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName);

					if (listI[0].Compare("add-int") == 0) {
						varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR1, nInstIndex);
						it->SetInputRegInfo(strInputR1, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
						it->CheckInputRegStrongType(strInputR0, CString("int"));
						it->CheckInputRegStrongType(strInputR1, CString("int"));
					}
					else if (listI[0].Compare("add-long") == 0) {
						varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR1, nInstIndex);
						it->SetInputRegInfo(strInputR1, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
						it->CheckInputRegStrongType(strInputR0, CString("long"));
						it->CheckInputRegStrongType(strInputR1, CString("long"));
					}
					else if (listI[0].Compare("add-float") == 0) {
						varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR1, nInstIndex);
						it->SetInputRegInfo(strInputR1, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);

						it->CheckInputRegStrongType(strInputR0, CString("float"));
						it->CheckInputRegStrongType(strInputR1, CString("float"));
					}
					else if (listI[0].Compare("add-int/lit16") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("short"));
					}
					else if (listI[0].Compare("add-int/lit8") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("char"));
					}
					else {
						printf("%s ָ������д���\n", strInstString);
					}
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

					CString strInputR0 = listI[1];		CString strInputR1 = listI[2];
					CString strOutputR0 = listI[1];
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR1, nInstIndex);
					it->SetInputRegInfo(strInputR1, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					it->SetOutputRegInfo(strOutputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName);

					if (listI[0].Compare("sub-int/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("int"));
						it->CheckInputRegStrongType(strInputR1, CString("int"));
					}
					else if (listI[0].Compare("sub-long/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("long"));
						it->CheckInputRegStrongType(strInputR1, CString("long"));
					}
					else if (listI[0].Compare("sub-float/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("float"));
						it->CheckInputRegStrongType(strInputR1, CString("float"));
					}
					else if (listI[0].Compare("sub-double/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("double"));
						it->CheckInputRegStrongType(strInputR1, CString("double"));
					}
					else {
						printf("%s ָ������д���\n", strInstString);
					}
				}
				else if (listI.size() == 4) {

					CString strInputR0 = listI[2];		CString strInputR1 = listI[3];
					CString strOutputR0 = listI[1];
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					it->SetOutputRegInfo(strOutputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName);
					varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR1, nInstIndex);
					it->SetInputRegInfo(strInputR1, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);

					if (listI[0].Compare("sub-int") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("int"));
						it->CheckInputRegStrongType(strInputR1, CString("int"));
					}
					else if (listI[0].Compare("sub-long") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("long"));
						it->CheckInputRegStrongType(strInputR1, CString("long"));
					}
					else if (listI[0].Compare("sub-float") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("float"));
						it->CheckInputRegStrongType(strInputR1, CString("float"));
					}
					else {
						printf("%s ָ������д���\n", strInstString);
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

					CString strInputR0 = listI[1];		CString strInputR1 = listI[2];
					CString strOutputR0 = listI[1];
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR1, nInstIndex);
					it->SetInputRegInfo(strInputR1, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					it->SetOutputRegInfo(strOutputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName);

					if (listI[0].Compare("mul-int/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("int"));
						it->CheckInputRegStrongType(strInputR1, CString("int"));
					}
					else if (listI[0].Compare("mul-long/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("long"));
						it->CheckInputRegStrongType(strInputR1, CString("long"));
					}
					else if (listI[0].Compare("mul-float/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("float"));
						it->CheckInputRegStrongType(strInputR1, CString("float"));
					}
					else if (listI[0].Compare("mul-double/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("double"));
						it->CheckInputRegStrongType(strInputR1, CString("double"));
					}
					else {
						printf("%s ָ������д���\n", strInstString);
					}
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

					CString strInputR0 = listI[1];		CString strInputR1 = listI[2];
					CString strOutputR0 = listI[1];
					CRegVariablePair varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR0, nInstIndex);
					it->SetInputRegInfo(strInputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					varPair = SegmentListRetrodictGetRegInfo(listInstIndex, i, strInputR1, nInstIndex);
					it->SetInputRegInfo(strInputR1, varPair.strStrongType, varPair.listWeakType, varPair.strValName, varPair.belongInstIndex);
					it->SetOutputRegInfo(strOutputR0, varPair.strStrongType, varPair.listWeakType, varPair.strValName);

					if (listI[0].Compare("div-int/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("int"));
						it->CheckInputRegStrongType(strInputR1, CString("int"));
					}
					else if (listI[0].Compare("div-long/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("long"));
						it->CheckInputRegStrongType(strInputR1, CString("long"));
					}
					else if (listI[0].Compare("div-float/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("float"));
						it->CheckInputRegStrongType(strInputR1, CString("float"));
					}
					else if (listI[0].Compare("div-double/2addr") == 0) {
						it->CheckInputRegStrongType(strInputR0, CString("double"));
						it->CheckInputRegStrongType(strInputR1, CString("double"));
					}
					else {
						printf("%s ָ������д���\n", strInstString);
					}
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
			}
			else if (strCmd.Find("catch") == 0) {
				//e�Ĵ���
				CString strOutputR0 = listI[1];
				it->SetOutputRegInfo(strOutputR0, "", listI[2], "");
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

	return true;
}

bool CJavaMethod::SegmentVariableRecursion(std::vector<int> listSegIndex, int nIndex) {

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
			SegmentVariableRecursion(listSegIndex, SSI);
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
			SegmentVariableRecursion(listSegIndex, SSI);
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
			SegmentVariableRecursion(listSegIndex, SSI);
		}
		else {
			printf("14�޷��ҵ������е���ʼλ��\n");
		}
		SSI = GetSegmentIndexFromName(strIfnLabel);
		if (SSI >= 0) {
			SegmentVariableRecursion(listSegIndex, SSI);
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
						SegmentVariableRecursion(listSegIndex, SSI);
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
			SegmentVariableRecursion(listSegIndex, SSI);
		}
		else {
			printf("17�޷��ҵ������е���ʼλ��\n");
		}
	}
	break;
	case CJavaMethodSegment::nextReturn:		// ͨ��return���صĶ�
	{
		return SegmentListInstRebuild(listSegIndex);
	}
	break;
	case CJavaMethodSegment::nextTryCatch:		// ͨ��TryCatch��֧
	{
		CString strTryLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("try->"));
		CString strDefaultLabel = SegmentNextGetOneLabel(listMethodSegment[nIndex].listStrNext, CString("default->"));

		std::vector<CTryCatchNode>::iterator ittcn;
		for (ittcn = listCatchData.begin(); ittcn != listCatchData.end(); ittcn++) {
			std::vector<CCatchItemNode>::iterator itcit;
			if (strTryLabel.Compare((*ittcn).strName) == 0) { // �ҵ���Ӧ��Try�ṹ��

															  // try��֧
				int SsiStart = GetSegmentIndexFromName((*ittcn).strStartLabel);
				if (SsiStart >= 0) {
					SegmentVariableRecursion(listSegIndex, SsiStart);
				}
				else {
					printf("181�޷��ҵ������е���ʼλ��\n");
				}


				// catch��֧
				for (itcit = (*ittcn).listCatch.begin(); itcit != (*ittcn).listCatch.end(); itcit++) {
					int SSI = GetSegmentIndexFromName((*itcit).strCatchLabel);
					if (SSI >= 0) {
						SegmentVariableRecursion(listSegIndex, SSI);
					}
					else {
						printf("18�޷��ҵ������е���ʼλ��\n");
					}
				}
				// final��֧
				if (!(*ittcn).strFinallyLabel.IsEmpty()) {
					int SSI = GetSegmentIndexFromName((*ittcn).strFinallyLabel);
					if (SSI >= 0) {
						SegmentVariableRecursion(listSegIndex, SSI);
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
			SegmentVariableRecursion(listSegIndex, SSI);
		}
		else {
			printf("20�޷��ҵ������е���ʼλ��\n");
		}
	}
	break;
	case CJavaMethodSegment::nextThrow:			// ���Ҳ�ǽ�������
	{
		// ���õ�β����
		return SegmentListInstRebuild(listSegIndex);
	}
	break;
	default:
	{
		printf("û��ʹ�ù�������\n");
	}
	}

	return true;
}

CRegVariablePair CJavaMethod::GetRegVariablePair(CString strRegInfo) {

	int nFindDot = strRegInfo.Find("-");

	if (nFindDot > 0) {
		CString strInstIndex = strRegInfo.Left(nFindDot).Trim();
		CString strRegName = strRegInfo.Right(strRegInfo.GetLength() - nFindDot - 1).Trim();

		int nOutInst = atoi(strInstIndex);
		std::vector<CJavaInstructItem>::iterator itInst = GetOriginalInstruct(nOutInst);
		if (itInst == this->listOriginalInstruct.end()) {
			printf("�Ҳ�����ָ��\n");
			while (1) {};
		}

		for (unsigned int i = 0; i < itInst->regInfo.listIreg.size(); i++) {
			if (itInst->regInfo.listIreg[i].strRegName == strRegName) {
				return itInst->regInfo.listIreg[i];
			}
		}
		printf("������Ĵ����б����Ҳ�����ָ��\n");
		while (1) {};
	}
	else {
		int nOutInst = atoi(strRegInfo);
		std::vector<CJavaInstructItem>::iterator itInst = GetOriginalInstruct(nOutInst);
		if (itInst == this->listOriginalInstruct.end()) {
			printf("�Ҳ�����ָ��\n");
			while (1) {};
		}
		return itInst->regInfo.outReg;
	}
}

// ͨ���ݹ鷽ʽ���ݳ����мĴ������ܵ�����
void CJavaMethod::SegmentVariableDefine(void) {

	std::vector<int> listSegIndex;

	// ������ɨ�������б�����������ϵ����������
	listSegIndex.clear();
	int SSI = GetSegmentIndexFromName(CString("main"));
	if (SSI >= 0) {
		SegmentVariableRecursion(listSegIndex, SSI);
	}
	else {
		printf("21�޷��ҵ������е���ʼλ��\n");
	}


	// ��������������� ������ϵ
	// �����������������������
	// 1��ɨ������ָ�������Ĵ����б�������Ĵ����������б����
	printf("��ʾ�Ĵ������ù�ϵ�� ��ʾ����ָ�������Ĵ�����Ҫ��������ָ��\n");
	for (unsigned int i = 0; i < this->listOriginalInstruct.size(); i++) {
		printf("%4d --> ", this->listOriginalInstruct[i].nSerialNumber);
		for (unsigned int j = 0; j < this->listOriginalInstruct[i].regInfo.listIreg.size(); j++) {
			printf("%s(%s):", this->listOriginalInstruct[i].regInfo.listIreg[j].strRegName, this->listOriginalInstruct[i].regInfo.listIreg[j].strGetVarType());
			for (unsigned int k = 0; k < this->listOriginalInstruct[i].regInfo.listIreg[j].listQuotedInstIndex.size(); k++) {
				printf("%d,", this->listOriginalInstruct[i].regInfo.listIreg[j].listQuotedInstIndex[k]);
			}
			if (this->listOriginalInstruct[i].regInfo.listIreg[j].listQuotedInstIndex.size() == 0) {
				printf("���ָ�������Ĵ����Ҳ���������\n");
				while (1) {};
			}

			printf("; ");
		}
		printf("\n");
	}

	// ����Ĵ�������
	std::vector<std::vector<int>> listArrayOutregType;
	listArrayOutregType.clear();

	// ����������Ĵ����ϲ�
	for (unsigned int i = 0; i < this->listOriginalInstruct.size(); i++) {
		if (this->listOriginalInstruct[i].regInfo.listIreg.size() > 0) {	// �����ݼĴ�������ô�����ʸ������Ĵ�������
			for (unsigned int j = 0; j < this->listOriginalInstruct[i].regInfo.listIreg.size(); j++) {
				// this->listOriginalInstruct[i].regInfo.listIreg[j].strRegName ����ǼĴ�������
				// this->listOriginalInstruct[i].regInfo.listIreg[j].listRefedInstIndex ��������üĴ����б��������϶�����ͬһ��

				bool bFindOverlap = false;
				for (unsigned int m = 0; m < listArrayOutregType.size(); m++) {
					for (unsigned int k = 0; k < this->listOriginalInstruct[i].regInfo.listIreg[j].listQuotedInstIndex.size(); k++) {
						if (find(listArrayOutregType[m].begin(), listArrayOutregType[m].end(), this->listOriginalInstruct[i].regInfo.listIreg[j].listQuotedInstIndex[k]) != listArrayOutregType[m].end()) {
							listArrayOutregType[m].insert(listArrayOutregType[m].begin(),
								this->listOriginalInstruct[i].regInfo.listIreg[j].listQuotedInstIndex.begin(),
								this->listOriginalInstruct[i].regInfo.listIreg[j].listQuotedInstIndex.end());
							bFindOverlap = true;
							break;
						}
					}
					if (bFindOverlap) {
						break;
					}
				}
				// û���ҵ�������һ��
				if (!bFindOverlap) {
					listArrayOutregType.push_back(this->listOriginalInstruct[i].regInfo.listIreg[j].listQuotedInstIndex);
				}
			}
		}
	}

	// ��������
	printf("��ʾӦ�ö������±�����λ�ã�ͬһ�б�ʾ�������Ϊͬһ��\n");
	for (unsigned int m = 0; m < listArrayOutregType.size(); m++) {
		// ����ȥ��
		sort(listArrayOutregType[m].begin(), listArrayOutregType[m].end());
		std::vector<int>::iterator iter = std::unique(listArrayOutregType[m].begin(), listArrayOutregType[m].end());
		listArrayOutregType[m].erase(iter, listArrayOutregType[m].end());
#if 1
		for (unsigned int s = 0; s < listArrayOutregType[m].size(); s++) {
			printf("%d, ", listArrayOutregType[m][s]);
		}
		printf("\n");
#endif
	}

	// �Ա������и�ֵ
	int nVarIndexStart = 1;
	for (unsigned int m = 0; m < listArrayOutregType.size(); m++) {

		if (listArrayOutregType[m].size() > 0) {
			if (listArrayOutregType[m][0] < 0) {
				// ����ʹ�ú������������������Ҫ���¶��壬��Ϊ��������p�Ĵ�����ͨ�üĴ�����û�й�ϵ
			}
			else {

				CString strValueName;

				strValueName.Format("var%d", nVarIndexStart++);

				// ͳ��һ�������������˶��ٴ�
				int nValueBerefCount = 0;
				// outReg.listQuotePosition ������汣���������Ĵ�������Щ����ָ�����ù�

				// ����������ٴ���Ϊ���ʹ�ã� Ҳ���Ǳ���ֵ���ٴΣ�
				int nValueOutputCount = 0;

				// ��һ����ͬ������Ĵ�������
				for (unsigned int s = 0; s < listArrayOutregType[m].size(); s++) {
					std::vector<CJavaInstructItem>::iterator itInst = GetOriginalInstruct(listArrayOutregType[m][s]);
					if (itInst == this->listOriginalInstruct.end()) {
						printf("�Ҳ�����ָ��\n");
						while (1) {};
					}
					nValueBerefCount += itInst->regInfo.outReg.listQuotePosition.size();
					nValueOutputCount++;
				}

				// ����������Ĵ����϶���ֵ
				if (listArrayOutregType[m].size() > 0) {

					CString strType;

					for (unsigned int s = 0; s < listArrayOutregType[m].size(); s++) {
						std::vector<CJavaInstructItem>::iterator itInst = GetOriginalInstruct(listArrayOutregType[m][s]);
						if (itInst == this->listOriginalInstruct.end()) {
							printf("�Ҳ�����ָ��\n");
							while (1) {};
						}
						// ������ֵ
						itInst->regInfo.outReg.strValName = strValueName;
						itInst->regInfo.outReg.nOutregBeQuoteCount = nValueBerefCount;
						itInst->regInfo.outReg.nOutregAssignedCount = nValueOutputCount;

						if (strType.IsEmpty()) {
							strType = itInst->regInfo.outReg.strGetVarType();
						}
					}

					// ���뽫��������ȫ���б�
					CVariableItem cVariableItem;
					cVariableItem.clear();
					cVariableItem.strName = strValueName;
					cVariableItem.strTypeName = strType;
					listVariableItem.push_back(cVariableItem);
				}
			}
		}
		else {
			printf("���ش���\n");
			while (1) {};
		}
	}

	// ��ӡ�����б���������
	// ����Ƿ����б�����������
	printf("�������ָ�������Ĵ����� �Ĵ����� ������ �����õĴ���\n");
	for (unsigned int i = 0; i < this->listOriginalInstruct.size(); i++) {

		if ((this->listOriginalInstruct[i].regInfo.outReg.listQuotePosition.size() != 0) && (this->listOriginalInstruct[i].regInfo.outReg.nOutregBeQuoteCount != 0)) {
			printf("%4d %s %s(%s) used %d {",
				this->listOriginalInstruct[i].nSerialNumber,
				this->listOriginalInstruct[i].regInfo.outReg.strRegName,
				this->listOriginalInstruct[i].regInfo.outReg.strValName,
				this->listOriginalInstruct[i].regInfo.outReg.strGetVarType(),
				this->listOriginalInstruct[i].regInfo.outReg.nOutregBeQuoteCount);

			for (unsigned int j = 0; j < this->listOriginalInstruct[i].regInfo.outReg.listQuotePosition.size(); j++) {
				printf("%s, ", this->listOriginalInstruct[i].regInfo.outReg.listQuotePosition[j]);
			}
			printf("}\n");
		}
		else {
			printf("%4d %s\n", this->listOriginalInstruct[i].nSerialNumber, this->listOriginalInstruct[i].regInfo.outReg.strRegName);
		}
	}

	printf("���ͬ�����ͼĴ����� ��� ���� �Ĵ��������ͣ�������ʾ��ÿ���������϶���ͬһ�����͵�\n");
	printf("xx-xx �������ָ�������Ĵ����ϣ�����һ��������Ĵ����ϣ�[ǿ������]��(������)\n");
	for (unsigned int m = 0; m < listArrayOutregType.size(); m++) {
		// ����ȥ��
		sort(listArrayOutregType[m].begin(), listArrayOutregType[m].end());
		std::vector<int>::iterator iter = std::unique(listArrayOutregType[m].begin(), listArrayOutregType[m].end());
		listArrayOutregType[m].erase(iter, listArrayOutregType[m].end());

		std::vector<CString> listSameTypeRegInfo;
		listSameTypeRegInfo.clear();

		for (unsigned int s = 0; s < listArrayOutregType[m].size(); s++) {
			//printf("%d, ", listArrayOutregType[m][s]);
			if (listArrayOutregType[m][s] >= 0) {	// ���������Ĵ���ͬ������
				int nCurrentInstIndex = listArrayOutregType[m][s];

				CString strInstIndex;
				strInstIndex.Format("%d", nCurrentInstIndex);
				listSameTypeRegInfo.push_back(strInstIndex);

				std::vector<CJavaInstructItem>::iterator itInst = GetOriginalInstruct(nCurrentInstIndex);
				if (itInst == this->listOriginalInstruct.end()) {
					printf("�Ҳ�����ָ��\n");
					while (1) {};
				}
				for (unsigned int j = 0; j < itInst->regInfo.outReg.listQuotePosition.size(); j++) {
					listSameTypeRegInfo.push_back(itInst->regInfo.outReg.listQuotePosition[j]);
				}

			}
		}

		// ������ʾ����ǿ�����ͣ�Ȼ������������һ�µ�����ʲô���ͣ��������֮��Ҫ�����мĴ�����ǿ�������޸��ɹ�
		if (listSameTypeRegInfo.size() > 0) {
			for (unsigned int k = 0; k < listSameTypeRegInfo.size(); k++) {

				CString strLabel = listSameTypeRegInfo[k];
				CRegVariablePair tmpRegInfo;
				if (strLabel.Find(CString("-")) > 0) {
					tmpRegInfo = GetRegVariablePair(strLabel);
					printf("%s[%s](", strLabel, tmpRegInfo.strStrongType);
					for (unsigned int d = 0; d < tmpRegInfo.listWeakType.size(); d++) {
						printf("%s,", tmpRegInfo.listWeakType[d]);
					}
					printf(");");
				}
				else {
					tmpRegInfo = GetRegVariablePair(strLabel);
					tmpRegInfo = GetRegVariablePair(strLabel);
					printf("%s[%s](", strLabel, tmpRegInfo.strStrongType);
					for (unsigned int d = 0; d < tmpRegInfo.listWeakType.size(); d++) {
						printf("%s,", tmpRegInfo.listWeakType[d]);
					}
					printf(");");
				}
			}
			printf("\n");
		}
	}

}
