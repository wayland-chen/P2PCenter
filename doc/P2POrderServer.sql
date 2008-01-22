CREATE TABLE [Channel] (
	[ID] [int] IDENTITY (1, 1) NOT NULL ,
	[agent] [nvarchar] (50) COLLATE Chinese_PRC_CI_AS NOT NULL ,
	[upuserid] [int] NOT NULL CONSTRAINT [DF_Channel_upuserid] DEFAULT (0),
	[userid] [int] NOT NULL CONSTRAINT [DF_Channel_userid] DEFAULT (0),
	[nodename] [nvarchar] (255) COLLATE Chinese_PRC_CI_AS NULL ,
	[filehash] [nvarchar] (50) COLLATE Chinese_PRC_CI_AS NOT NULL ,
	[filesize] [bigint] NOT NULL CONSTRAINT [DF_Channel_filesize] DEFAULT (0),
	[filename] [nvarchar] (255) COLLATE Chinese_PRC_CI_AS NULL ,
	[appenddate] [datetime] NULL CONSTRAINT [DF_Channel_appenddate] DEFAULT (getdate()),
	[updatedate] [datetime] NULL CONSTRAINT [DF_Channel_updatedate] DEFAULT (getdate()),
	CONSTRAINT [PK_Channel] PRIMARY KEY  CLUSTERED 
	(
		[userid],
		[filehash],
		[filesize]
	)  ON [PRIMARY] 
) ON [PRIMARY]
GO


