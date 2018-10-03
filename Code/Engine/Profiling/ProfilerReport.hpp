#pragma once

struct ProfilerNode_t;

struct ReportNode_t
{
	const char *tag;
	unsigned int numCalls;
	float totalTime;
	float selfTime;
	float averageTotalTime;
	float averageSelfTime;
	float percentOfFrameTime;
	float percentOfSelfTime;

	ReportNode_t *parent;
	ReportNode_t *children;
	ReportNode_t *next;
	ReportNode_t *prev;
};

extern ReportNode_t *g_reportTree;
extern bool g_isTreeView;

class ProfilerReport
{
public:
	ReportNode_t *m_activeNode;
	float m_totalFrameTime;

	ProfilerReport(ProfilerNode_t *frame);
	~ProfilerReport();

	void CreateTreeView();
	void CreateFlatView();
	void SortByTotalTime();
	void RecursiveSortColumnByTotalTime(ReportNode_t *node);
	void RecursiveSortRowByTotalTime(ReportNode_t *node);
	void SortBySelfTime();
	void Log();
	void LogNode();
	bool DoesTagExistInSiblings(ReportNode_t *node, const char *tag);
	bool DoesTagExistInImmediateChildren(ReportNode_t *node, const char *tag);
	ReportNode_t *GetNodeWithTagInCurrentFlatViewTree(const char *tag);
	ReportNode_t *GetSiblingNodeWithTag(ReportNode_t *node, const char *tag);
	ReportNode_t *GetImmediateChildWithTag(ReportNode_t *node, const char *tag);
	ReportNode_t *GetLastFlatViewNodeInCurrentTree();
	void PushTreeViewNode(ProfilerNode_t *node);
	void PushFlatViewNode(ProfilerNode_t *node);
	void PopReportNode();
	void RecursivePushTreeViewReportNode(ProfilerNode_t *node);
	void RecursivePushFlatViewReportNode(ProfilerNode_t *node);
	void RecursiveDeleteReportNode(ReportNode_t *node);
	int GetLengthOfSiblings(ReportNode_t *node);
};