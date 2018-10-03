#include "Engine/Profiling/ProfilerReport.hpp"
#include "Engine/Profiling/Profiler.hpp"
#include <string.h>

#ifdef PROFILED_BUILD

ProfilerNode_t *g_frameTree = nullptr;
ReportNode_t *g_reportTree = nullptr;
bool g_isTreeView = false;

ProfilerReport::ProfilerReport(ProfilerNode_t *frame)
	: m_activeNode(nullptr)
{
	g_frameTree = frame;
	m_totalFrameTime = (float)GetTotalTime(frame);
	RecursiveDeleteReportNode(g_reportTree);
	g_reportTree = nullptr;
}

ProfilerReport::~ProfilerReport()
{
}

void ProfilerReport::CreateTreeView()
{
	g_isTreeView = true;
	RecursivePushTreeViewReportNode(g_frameTree);
}

void ProfilerReport::CreateFlatView()
{
	g_isTreeView = false;
	RecursivePushFlatViewReportNode(g_frameTree);
}

void ProfilerReport::SortByTotalTime()
{
	RecursiveSortColumnByTotalTime(g_reportTree);
}

void ProfilerReport::RecursiveSortColumnByTotalTime(ReportNode_t *node)
{
	bool isSorted = false;
	while (!isSorted)
	{
		isSorted = true;
		ReportNode_t *currentNode = node;

		while (currentNode != nullptr)
		{
			ReportNode_t *nextNode = currentNode->children;
			if (nextNode != nullptr && nextNode->totalTime > currentNode->totalTime)
			{
				ReportNode_t *parentNode = nextNode->parent;
				ReportNode_t *childNode = nextNode->children;

				if (currentNode->parent == nextNode)
					nextNode->parent = currentNode;
				else
				{
					nextNode->parent = currentNode->parent;
					if (nextNode->parent != nullptr)
						nextNode->parent->children = nextNode;
				}

				if (currentNode->children == nextNode)
					nextNode->children = currentNode;
				else
				{
					nextNode->children = currentNode->children;
					if (nextNode->children != nullptr)
						nextNode->children->parent = nextNode;
				}

				if (parentNode == currentNode)
					currentNode->parent = nextNode;
				else
				{
					currentNode->parent = parentNode;
					if (nextNode->parent != nullptr)
						currentNode->parent->children = currentNode;
				}

				if (childNode == currentNode)
					currentNode->children = nextNode;
				else
				{
					currentNode->children = childNode;
					if (currentNode->children != nullptr)
						currentNode->children->parent = currentNode;
				}


				isSorted = false;
			}

			currentNode = currentNode->children;
		}
	}

	ReportNode_t *currentNode = node;
	while (currentNode != nullptr)
	{
		RecursiveSortRowByTotalTime(currentNode);
		currentNode = currentNode->children;
	}
}

void ProfilerReport::RecursiveSortRowByTotalTime(ReportNode_t *node)
{
	ReportNode_t *currentHead = node;
	int listLength = GetLengthOfSiblings(node);
	for (int numIterations = 0; numIterations < listLength; numIterations++)
	{
		ReportNode_t *currentNode = currentHead;

		if (node == node->next)
			break;

		for (int nodeCount = 0; nodeCount < listLength; nodeCount++)
		{
			ReportNode_t *nextNode = currentNode->next;
			if (nextNode->totalTime > currentNode->totalTime)
			{
				ReportNode_t *parentNode = nextNode->prev;
				ReportNode_t *childNode = nextNode->next;

				if (nextNode == currentHead)
				{
					if (currentNode->parent != nullptr)
					{
						currentNode->parent->children = currentNode;
						currentHead = currentNode;
					}
				}

				if (currentNode == currentHead)
				{
					if (nextNode->parent != nullptr)
					{
						nextNode->parent->children = nextNode;
						currentHead = nextNode;
					}
				}

				if (currentNode->prev == nextNode)
					nextNode->prev = currentNode;
				else
				{
					nextNode->prev = currentNode->prev;
					if (nextNode->prev != nullptr)
						nextNode->prev->next = nextNode;
				}

				if (currentNode->next == nextNode)
					nextNode->next = currentNode;
				else
				{
					nextNode->next = currentNode->next;
					if (nextNode->next != nullptr)
						nextNode->next->prev = nextNode;
				}

				if (parentNode == currentNode)
					currentNode->prev = nextNode;
				else
				{
					currentNode->prev = parentNode;
					if (nextNode->prev != nullptr)
						currentNode->prev->next = currentNode;
				}

				if (childNode == currentNode)
					currentNode->next = nextNode;
				else
				{
					currentNode->next = childNode;
					if (currentNode->next != nullptr)
						currentNode->next->prev = currentNode;
				}
			}

			currentNode = currentNode->next;
		}
	}

	ReportNode_t *currentNode = node->next;
	while (currentNode != node)
	{
		if (currentNode->children != nullptr)
			RecursiveSortColumnByTotalTime(currentNode);
		currentNode = currentNode->next;
	}
}

void ProfilerReport::SortBySelfTime()
{
	bool isSorted = false;
	while (!isSorted)
	{
		isSorted = true;
		ReportNode_t *currentNode = g_reportTree;

		while (currentNode != nullptr)
		{
			ReportNode_t *nextNode = currentNode->children;
			if (nextNode != nullptr && nextNode->selfTime > currentNode->selfTime)
			{
				ReportNode_t *parentNode = nextNode->parent;
				ReportNode_t *childNode = nextNode->children;

				if (nextNode == g_reportTree)
					g_reportTree = currentNode;

				if (currentNode == g_reportTree)
					g_reportTree = nextNode;

				if (currentNode->parent == nextNode)
					nextNode->parent = currentNode;
				else
				{
					nextNode->parent = currentNode->parent;
					if (nextNode->parent != nullptr)
						nextNode->parent->children = nextNode;
				}

				if (currentNode->children == nextNode)
					nextNode->children = currentNode;
				else
				{
					nextNode->children = currentNode->children;
					if (nextNode->children != nullptr)
						nextNode->children->parent = nextNode;
				}

				if (parentNode == currentNode)
					currentNode->parent = nextNode;
				else
				{
					currentNode->parent = parentNode;
					if (nextNode->parent != nullptr)
						currentNode->parent->children = currentNode;
				}

				if (childNode == currentNode)
					currentNode->children = nextNode;
				else
				{
					currentNode->children = childNode;
					if (currentNode->children != nullptr)
						currentNode->children->parent = currentNode;
				}


				isSorted = false;
			}

			currentNode = currentNode->children;
		}
	}
}

void ProfilerReport::Log()
{

}

void ProfilerReport::LogNode()
{
	
}

bool ProfilerReport::DoesTagExistInSiblings(ReportNode_t *node, const char *tag)
{
	if (node == nullptr)
		return false;

	else
	{
		ReportNode_t *currentNode = node;

		do 
		{
			if (strcmp(tag, currentNode->tag) == 0)
				return true;

			currentNode = currentNode->next;
		} while (currentNode != node);
		
		return false;
	}
}

bool ProfilerReport::DoesTagExistInImmediateChildren(ReportNode_t *node, const char *tag)
{
	if (node == nullptr)
		return false;

	return DoesTagExistInSiblings(node->children, tag);
}

ReportNode_t * ProfilerReport::GetNodeWithTagInCurrentFlatViewTree(const char *tag)
{
	if (g_reportTree == nullptr)
		return nullptr;

	ReportNode_t *currentNode = g_reportTree;

	while(currentNode != nullptr)
	{
		if (strcmp(tag, currentNode->tag) == 0)
		{
			return currentNode;
		}
		currentNode = currentNode->children;
	}

	return nullptr;
}

ReportNode_t * ProfilerReport::GetSiblingNodeWithTag(ReportNode_t *node, const char *tag)
{
	if (node == nullptr)
		return nullptr;

	else
	{
		ReportNode_t *currentNode = node;

		do
		{
			if (strcmp(tag, currentNode->tag) == 0)
				return currentNode;

			currentNode = currentNode->next;
		} while (currentNode != node);

		return nullptr;
	}
}

ReportNode_t * ProfilerReport::GetImmediateChildWithTag(ReportNode_t *node, const char *tag)
{
	if (node == nullptr)
		return nullptr;

	return GetSiblingNodeWithTag(node->children, tag);
}

ReportNode_t * ProfilerReport::GetLastFlatViewNodeInCurrentTree()
{
	if (g_reportTree == nullptr)
		return nullptr;

	ReportNode_t *currentNode = g_reportTree;

	while (currentNode != nullptr)
	{
		if (currentNode->children == nullptr)
		{
			return currentNode;
		}
		currentNode = currentNode->children;
	}

	return nullptr;
}

void ProfilerReport::PushTreeViewNode(ProfilerNode_t *node)
{
	bool doesTagExistInSiblings = DoesTagExistInImmediateChildren(m_activeNode, node->description);
	if (!doesTagExistInSiblings)
	{
		ReportNode_t *newReportNode = new ReportNode_t();
		newReportNode->tag = node->description;
		newReportNode->numCalls = 1;
		newReportNode->totalTime = (float)GetTotalTime(node);
		newReportNode->selfTime = (float)GetSelfTime(node);
		newReportNode->averageTotalTime = newReportNode->totalTime;
		newReportNode->averageSelfTime = newReportNode->selfTime;
		newReportNode->percentOfFrameTime = newReportNode->totalTime / m_totalFrameTime;
		newReportNode->percentOfSelfTime = newReportNode->selfTime / m_totalFrameTime;
		newReportNode->parent = nullptr;
		newReportNode->children = nullptr;
		newReportNode->next = newReportNode;
		newReportNode->prev = newReportNode;

		if (m_activeNode == nullptr)
		{
			m_activeNode = newReportNode;
			g_reportTree = m_activeNode;
		}
		else
		{
			if (m_activeNode->children == nullptr)
			{
				m_activeNode->children = newReportNode;
			}
			else
			{
				newReportNode->next = m_activeNode->children;
				newReportNode->prev = m_activeNode->children->prev;
				newReportNode->prev->next = newReportNode;
				newReportNode->next->prev = newReportNode;
			}

			newReportNode->parent = m_activeNode;
		}

		m_activeNode = newReportNode;
	}
	else
	{
		ReportNode_t *oldReportNode = GetImmediateChildWithTag(m_activeNode, node->description);
		oldReportNode->numCalls++;
		oldReportNode->totalTime += (float)GetTotalTime(node);
		oldReportNode->selfTime += (float)GetSelfTime(node);
		oldReportNode->averageTotalTime = oldReportNode->totalTime / oldReportNode->numCalls;
		oldReportNode->averageSelfTime = oldReportNode->selfTime / oldReportNode->numCalls;
		oldReportNode->percentOfFrameTime = oldReportNode->totalTime / m_totalFrameTime;
		oldReportNode->percentOfSelfTime = oldReportNode->selfTime / m_totalFrameTime;

		m_activeNode = oldReportNode;
	}
}

void ProfilerReport::PushFlatViewNode(ProfilerNode_t *node)
{
	ReportNode_t *oldReportNode = GetNodeWithTagInCurrentFlatViewTree(node->description);
	if (oldReportNode == nullptr)
	{
		ReportNode_t *newReportNode = new ReportNode_t();
		newReportNode->tag = node->description;
		newReportNode->numCalls = 1;
		newReportNode->totalTime = (float)GetTotalTime(node);
		newReportNode->selfTime = (float)GetSelfTime(node);
		newReportNode->averageTotalTime = newReportNode->totalTime;
		newReportNode->averageSelfTime = newReportNode->selfTime;
		newReportNode->percentOfFrameTime = newReportNode->totalTime / m_totalFrameTime;
		newReportNode->percentOfSelfTime = newReportNode->selfTime / m_totalFrameTime;
		
		if (g_reportTree == nullptr)
		{
			g_reportTree = newReportNode;
			newReportNode->parent = nullptr;
		}
		else
		{
			newReportNode->parent = GetLastFlatViewNodeInCurrentTree();
			newReportNode->parent->children = newReportNode;
		}
		newReportNode->children = nullptr;
		newReportNode->next = newReportNode;
		newReportNode->prev = newReportNode;
	}
	else
	{
		oldReportNode->numCalls++;
		oldReportNode->totalTime += (float)GetTotalTime(node);
		oldReportNode->selfTime += (float)GetSelfTime(node);
		oldReportNode->averageTotalTime = oldReportNode->totalTime / oldReportNode->numCalls;
		oldReportNode->averageSelfTime = oldReportNode->selfTime / oldReportNode->numCalls;
		oldReportNode->percentOfFrameTime = (float)oldReportNode->totalTime / m_totalFrameTime;
		oldReportNode->percentOfSelfTime = (float)oldReportNode->selfTime / m_totalFrameTime;
	}
}

void ProfilerReport::PopReportNode()
{
	m_activeNode = m_activeNode->parent;
}

void ProfilerReport::RecursivePushTreeViewReportNode(ProfilerNode_t *node)
{
	PushTreeViewNode(node);

	if (node->children != nullptr)
		RecursivePushTreeViewReportNode(node->children);

	PopReportNode();

	ProfilerNode_t *currentNode = node->next;
	while (currentNode != node)
	{
		PushTreeViewNode(currentNode);
		if (currentNode->children != nullptr)
			RecursivePushTreeViewReportNode(currentNode->children);
		PopReportNode();
		currentNode = currentNode->next;
	}
}

void ProfilerReport::RecursivePushFlatViewReportNode(ProfilerNode_t *node)
{
	PushFlatViewNode(node);

	if (node->children != nullptr)
		RecursivePushFlatViewReportNode(node->children);

	ProfilerNode_t *currentNode = node->next;
	while (currentNode != node)
	{
		PushFlatViewNode(currentNode);
		if (currentNode->children != nullptr)
			RecursivePushFlatViewReportNode(currentNode->children);
		currentNode = currentNode->next;
	}
}

void ProfilerReport::RecursiveDeleteReportNode(ReportNode_t *node)
{
	if (node == nullptr)
		return;

	if (node->children != nullptr)
	{
		RecursiveDeleteReportNode(node->children);
		node->children = nullptr;
	}

	ReportNode_t *headNode = node;
	ReportNode_t *currentNode = node->prev;
	while (currentNode != headNode)
	{
		if (currentNode->children != nullptr)
			RecursiveDeleteReportNode(currentNode->children);

		currentNode = currentNode->prev;

		currentNode->next->next = nullptr;
		currentNode->next->prev = nullptr;
		delete currentNode->next;
		currentNode->next = nullptr;

	}

	node->next = nullptr;
	node->prev = nullptr;
	delete node;
	node = nullptr;
}

int ProfilerReport::GetLengthOfSiblings(ReportNode_t *node)
{
	ReportNode_t *currentNode = node;
	int numNodes = 0;
	do 
	{
		numNodes++;
		currentNode = currentNode->next;
	} while (currentNode != node);

	return numNodes;
}

#endif