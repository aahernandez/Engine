#pragma once

class NetMessage;
class NetObject;
class NetObjectConnectionState;

typedef void	(*AppendCreateInfoCB)	(NetMessage*, void *localObj);
typedef void	(*AppendDestroyInfoCB)	(NetMessage*, void *localObj);
typedef void*	(*ProcessCreateInfoCB)	(NetMessage*, NetObject *obj);
typedef void	(*ProcessDestroyInfoCB)	(NetMessage*, void *localObj);
typedef void	(*CreateSnapshotCB)		(void *localObj);
typedef void	(*GetSnapshotCB)		(void *currentSnapshot, void *localObj);
typedef bool	(*AppendSnapshotCB)		(NetMessage*, NetObject*, NetObjectConnectionState *lastSentState);
typedef void	(*ProcessSnapshotCB)	(void *lastReceivedSnapshot, NetMessage*);
typedef void	(*ApplySnapshotCB)		(void *localObj, void *lastSnapshot, float relativeTime);
typedef void	(*AppendHostTime)		(NetMessage*);
typedef float	(*ProcessHostTime)		(NetMessage*);
typedef unsigned int	(*GetMaxSnapshotSize)	();

class NetObjectTypeDefinition
{
public:
	unsigned char m_typeID;
	AppendCreateInfoCB append_create_info;
	AppendDestroyInfoCB append_destroy_info;
	ProcessCreateInfoCB process_create_info;
	ProcessDestroyInfoCB process_destroy_info;
	ApplySnapshotCB apply_snapshot;
	GetSnapshotCB get_current_snapshot;
	AppendSnapshotCB append_snapshot;
	ProcessSnapshotCB process_snapshot;
	AppendHostTime append_host_time;
	ProcessHostTime process_host_time;
	GetMaxSnapshotSize get_max_snapshot_size;
};