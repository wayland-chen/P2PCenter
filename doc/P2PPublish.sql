CREATE TABLE [P2PPublish] (
	[MediaID] [int] NOT NULL ,
	[MediaAddrID] [int] NOT NULL ,
	[MediaName] [nvarchar] (50) COLLATE Chinese_PRC_CI_AS NOT NULL ,
	[FileName] [nvarchar] (255) COLLATE Chinese_PRC_CI_AS NOT NULL ,
	[FilePath] [nvarchar] (255) COLLATE Chinese_PRC_CI_AS NOT NULL ,
	[NodeName] [varchar] (50) COLLATE Chinese_PRC_CI_AS NULL CONSTRAINT [DF_P2PPublish_NodeName] DEFAULT ('test_node'),
	[FileHash] [varchar] (50) COLLATE Chinese_PRC_CI_AS NULL ,
	[FileSize] [bigint] NULL ,
	[ModifyDate] [numeric](18, 0) NULL CONSTRAINT [DF_P2PPublish_ModifyDate] DEFAULT (0),
	[PublishDate] [datetime] NULL CONSTRAINT [DF_P2PPublish_PublishDate] DEFAULT (getdate()),
	CONSTRAINT [PK_P2PPublish] PRIMARY KEY  NONCLUSTERED 
	(
		[MediaID],
		[MediaAddrID]
	)  ON [PRIMARY] 
) ON [PRIMARY]
GO


