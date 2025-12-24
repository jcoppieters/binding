#include "stdafx.h"
#include "AppNodeDbase_Util.h"

#include "NodeDatabase.h"

/***************************** private ****************************/

static 
inline BOOL checkNodeDatabase(
	  CNodeDatabase* pNodeDatabase)
{
	int nrOfNodes = 0;
	return (pNodeDatabase->NROfNodes(&nrOfNodes));	
}
/******************************************************************/


// Opzoeken van nodes met een bepaalde product code.
int CAppNodeDbase_Util::FindNodesWithProductID(
	CNodeDatabase *pNodeDatabase,
	int productID,
	CCanNode** ppNodes,
	int maxElements)
{
	int count = 0;

	if (TRUE == checkNodeDatabase(pNodeDatabase))
	{
		for (CNodeIterator it(pNodeDatabase->CreateNodeIterator());
			 !it.IsDone();
			 it.Next())
		{
			CCanNode* const pNode = it.CurrentItem();

			if (pNode->GetProductID()==productID)
			{
				if (count<(maxElements-1))
				{
					ppNodes[count] = pNode;
					count++;
				}
			}
		}
	}
	return count;
}

int CAppNodeDbase_Util::getCountNodesWithProductID(
	CNodeDatabase *pNodeDatabase,
	int productID)
{
	int retCountVal = 0;

	if (TRUE == checkNodeDatabase(pNodeDatabase))
	{
		for (CNodeIterator it(pNodeDatabase->CreateNodeIterator());
			 !it.IsDone();
			 it.Next())
		{
			CCanNode* const pNode = it.CurrentItem();
			if (pNode->GetProductID()==productID)
			{
				retCountVal++;
			}
		}
	}
	return retCountVal;
}