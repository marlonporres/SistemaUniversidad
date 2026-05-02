USE [Universidad]
GO
/****** Object:  Table [dbo].[alumnos]    Script Date: 5/2/2026 10:31:49 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[alumnos](
	[id] [int] IDENTITY(1,1) NOT NULL,
	[nombres] [varchar](100) NOT NULL,
	[apellidos] [varchar](100) NOT NULL,
	[carnet] [varchar](50) NULL,
 CONSTRAINT [PK_alumnos] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[alumnos_cursos]    Script Date: 5/2/2026 10:31:49 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[alumnos_cursos](
	[id] [int] IDENTITY(1,1) NOT NULL,
	[idAlumnos] [int] NULL,
	[idCursos] [int] NULL,
 CONSTRAINT [PK_alumnos_cursos] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[alumnos_seccion]    Script Date: 5/2/2026 10:31:49 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[alumnos_seccion](
	[id] [int] IDENTITY(1,1) NOT NULL,
	[alumno__id] [int] NULL,
	[seccion_id] [int] NULL,
 CONSTRAINT [PK_alumnos_seccion] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[cursos]    Script Date: 5/2/2026 10:31:49 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[cursos](
	[id] [int] IDENTITY(1,1) NOT NULL,
	[codigo_curso] [varchar](50) NOT NULL,
	[nombre] [varchar](50) NULL,
 CONSTRAINT [PK_cursos] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[secciones]    Script Date: 5/2/2026 10:31:49 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[secciones](
	[id] [int] IDENTITY(1,1) NOT NULL,
	[nombre] [varchar](50) NULL,
	[jornada] [varchar](50) NULL,
 CONSTRAINT [PK_secciones] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
ALTER TABLE [dbo].[alumnos_cursos]  WITH CHECK ADD  CONSTRAINT [FK_alumnos_cursos_alumnos] FOREIGN KEY([idAlumnos])
REFERENCES [dbo].[alumnos] ([id])
GO
ALTER TABLE [dbo].[alumnos_cursos] CHECK CONSTRAINT [FK_alumnos_cursos_alumnos]
GO
ALTER TABLE [dbo].[alumnos_cursos]  WITH CHECK ADD  CONSTRAINT [FK_alumnos_cursos_cursos] FOREIGN KEY([idCursos])
REFERENCES [dbo].[cursos] ([id])
GO
ALTER TABLE [dbo].[alumnos_cursos] CHECK CONSTRAINT [FK_alumnos_cursos_cursos]
GO
ALTER TABLE [dbo].[alumnos_seccion]  WITH CHECK ADD  CONSTRAINT [FK_alumnos_seccion_alumnos] FOREIGN KEY([alumno__id])
REFERENCES [dbo].[alumnos] ([id])
GO
ALTER TABLE [dbo].[alumnos_seccion] CHECK CONSTRAINT [FK_alumnos_seccion_alumnos]
GO
ALTER TABLE [dbo].[alumnos_seccion]  WITH CHECK ADD  CONSTRAINT [FK_alumnos_seccion_secciones] FOREIGN KEY([seccion_id])
REFERENCES [dbo].[secciones] ([id])
GO
ALTER TABLE [dbo].[alumnos_seccion] CHECK CONSTRAINT [FK_alumnos_seccion_secciones]
GO
