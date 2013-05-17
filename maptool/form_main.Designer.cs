namespace Maptool
{
    partial class frmMain
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null)) {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.panOuter = new System.Windows.Forms.Panel();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuFileNew = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuFileOpen = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuFileSave = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuFileSaveas = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuFileExit = new System.Windows.Forms.ToolStripMenuItem();
            this.editToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuEditMapsettings = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.toolZones = new System.Windows.Forms.ToolStripButton();
            this.toolLights = new System.Windows.Forms.ToolStripButton();
            this.splitRight = new System.Windows.Forms.SplitContainer();
            this.grid = new System.Windows.Forms.PropertyGrid();
            this.list = new System.Windows.Forms.ListView();
            this.columnHeader1 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.imgList = new System.Windows.Forms.ImageList(this.components);
            this.splitOuter = new System.Windows.Forms.SplitContainer();
            this.diaSave = new System.Windows.Forms.SaveFileDialog();
            this.diaOpen = new System.Windows.Forms.OpenFileDialog();
            this.toolObjects = new System.Windows.Forms.ToolStripButton();
            this.dropdownBGtype = new System.Windows.Forms.ToolStripComboBox();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.panMap = new Maptool.DoubleBufferPanel();
            this.panOuter.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            this.toolStrip1.SuspendLayout();
            this.splitRight.Panel1.SuspendLayout();
            this.splitRight.Panel2.SuspendLayout();
            this.splitRight.SuspendLayout();
            this.splitOuter.Panel1.SuspendLayout();
            this.splitOuter.Panel2.SuspendLayout();
            this.splitOuter.SuspendLayout();
            this.SuspendLayout();
            // 
            // panOuter
            // 
            this.panOuter.AutoScroll = true;
            this.panOuter.AutoScrollMargin = new System.Drawing.Size(50, 50);
            this.panOuter.BackColor = System.Drawing.Color.Silver;
            this.panOuter.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panOuter.Controls.Add(this.panMap);
            this.panOuter.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panOuter.Location = new System.Drawing.Point(0, 0);
            this.panOuter.Name = "panOuter";
            this.panOuter.Size = new System.Drawing.Size(563, 453);
            this.panOuter.TabIndex = 0;
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.editToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(746, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuFileNew,
            this.mnuFileOpen,
            this.mnuFileSave,
            this.mnuFileSaveas,
            this.toolStripSeparator1,
            this.mnuFileExit});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "&File";
            // 
            // mnuFileNew
            // 
            this.mnuFileNew.Name = "mnuFileNew";
            this.mnuFileNew.Size = new System.Drawing.Size(123, 22);
            this.mnuFileNew.Text = "&New";
            this.mnuFileNew.Click += new System.EventHandler(this.mnuFileNew_Click);
            // 
            // mnuFileOpen
            // 
            this.mnuFileOpen.Name = "mnuFileOpen";
            this.mnuFileOpen.Size = new System.Drawing.Size(123, 22);
            this.mnuFileOpen.Text = "&Open...";
            this.mnuFileOpen.Click += new System.EventHandler(this.mnuFileOpen_Click);
            // 
            // mnuFileSave
            // 
            this.mnuFileSave.Enabled = false;
            this.mnuFileSave.Name = "mnuFileSave";
            this.mnuFileSave.Size = new System.Drawing.Size(123, 22);
            this.mnuFileSave.Text = "&Save";
            // 
            // mnuFileSaveas
            // 
            this.mnuFileSaveas.Name = "mnuFileSaveas";
            this.mnuFileSaveas.Size = new System.Drawing.Size(123, 22);
            this.mnuFileSaveas.Text = "&Save As...";
            this.mnuFileSaveas.Click += new System.EventHandler(this.mnuFileSaveas_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(120, 6);
            // 
            // mnuFileExit
            // 
            this.mnuFileExit.Name = "mnuFileExit";
            this.mnuFileExit.Size = new System.Drawing.Size(123, 22);
            this.mnuFileExit.Text = "E&xit";
            this.mnuFileExit.Click += new System.EventHandler(this.mnuFileExit_Click);
            // 
            // editToolStripMenuItem
            // 
            this.editToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuEditMapsettings});
            this.editToolStripMenuItem.Name = "editToolStripMenuItem";
            this.editToolStripMenuItem.Size = new System.Drawing.Size(39, 20);
            this.editToolStripMenuItem.Text = "&Edit";
            // 
            // mnuEditMapsettings
            // 
            this.mnuEditMapsettings.Enabled = false;
            this.mnuEditMapsettings.Name = "mnuEditMapsettings";
            this.mnuEditMapsettings.Size = new System.Drawing.Size(143, 22);
            this.mnuEditMapsettings.Text = "Map &Settings";
            this.mnuEditMapsettings.Click += new System.EventHandler(this.mnuEditMapsettings_Click);
            // 
            // toolStrip1
            // 
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolZones,
            this.toolLights,
            this.toolObjects,
            this.toolStripSeparator2,
            this.dropdownBGtype});
            this.toolStrip1.Location = new System.Drawing.Point(0, 24);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(746, 25);
            this.toolStrip1.TabIndex = 1;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // toolZones
            // 
            this.toolZones.Image = global::Maptool.Properties.Resources.zones;
            this.toolZones.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolZones.Name = "toolZones";
            this.toolZones.Size = new System.Drawing.Size(59, 22);
            this.toolZones.Text = "Zones";
            this.toolZones.Click += new System.EventHandler(this.toolZones_Click);
            // 
            // toolLights
            // 
            this.toolLights.Image = global::Maptool.Properties.Resources.zones;
            this.toolLights.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolLights.Name = "toolLights";
            this.toolLights.Size = new System.Drawing.Size(59, 22);
            this.toolLights.Text = "Lights";
            this.toolLights.Click += new System.EventHandler(this.toolLights_Click);
            // 
            // splitRight
            // 
            this.splitRight.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitRight.Location = new System.Drawing.Point(0, 0);
            this.splitRight.Name = "splitRight";
            this.splitRight.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitRight.Panel1
            // 
            this.splitRight.Panel1.AutoScroll = true;
            this.splitRight.Panel1.Controls.Add(this.grid);
            // 
            // splitRight.Panel2
            // 
            this.splitRight.Panel2.AutoScroll = true;
            this.splitRight.Panel2.Controls.Add(this.list);
            this.splitRight.Size = new System.Drawing.Size(179, 453);
            this.splitRight.SplitterDistance = 238;
            this.splitRight.TabIndex = 5;
            // 
            // grid
            // 
            this.grid.Dock = System.Windows.Forms.DockStyle.Fill;
            this.grid.Location = new System.Drawing.Point(0, 0);
            this.grid.Name = "grid";
            this.grid.PropertySort = System.Windows.Forms.PropertySort.Categorized;
            this.grid.Size = new System.Drawing.Size(179, 238);
            this.grid.TabIndex = 0;
            this.grid.ToolbarVisible = false;
            this.grid.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.grid_PropertyValueChanged);
            // 
            // list
            // 
            this.list.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1});
            this.list.Dock = System.Windows.Forms.DockStyle.Fill;
            this.list.HideSelection = false;
            this.list.LargeImageList = this.imgList;
            this.list.Location = new System.Drawing.Point(0, 0);
            this.list.MultiSelect = false;
            this.list.Name = "list";
            this.list.Size = new System.Drawing.Size(179, 211);
            this.list.SmallImageList = this.imgList;
            this.list.TabIndex = 0;
            this.list.UseCompatibleStateImageBehavior = false;
            this.list.View = System.Windows.Forms.View.Tile;
            this.list.SelectedIndexChanged += new System.EventHandler(this.list_SelectedIndexChanged);
            this.list.Resize += new System.EventHandler(this.list_Resize);
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "Name";
            this.columnHeader1.Width = 184;
            // 
            // imgList
            // 
            this.imgList.ColorDepth = System.Windows.Forms.ColorDepth.Depth8Bit;
            this.imgList.ImageSize = new System.Drawing.Size(32, 32);
            this.imgList.TransparentColor = System.Drawing.Color.Transparent;
            // 
            // splitOuter
            // 
            this.splitOuter.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitOuter.Location = new System.Drawing.Point(0, 49);
            this.splitOuter.Name = "splitOuter";
            // 
            // splitOuter.Panel1
            // 
            this.splitOuter.Panel1.Controls.Add(this.panOuter);
            // 
            // splitOuter.Panel2
            // 
            this.splitOuter.Panel2.Controls.Add(this.splitRight);
            this.splitOuter.Size = new System.Drawing.Size(746, 453);
            this.splitOuter.SplitterDistance = 563;
            this.splitOuter.TabIndex = 5;
            // 
            // diaSave
            // 
            this.diaSave.Filter = "Maps|*.conf";
            // 
            // diaOpen
            // 
            this.diaOpen.Filter = "Maps|*.conf";
            // 
            // toolObjects
            // 
            this.toolObjects.Image = global::Maptool.Properties.Resources.zones;
            this.toolObjects.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolObjects.Name = "toolObjects";
            this.toolObjects.Size = new System.Drawing.Size(67, 22);
            this.toolObjects.Text = "Objects";
            this.toolObjects.Click += new System.EventHandler(this.toolObjects_Click);
            // 
            // dropdownBGtype
            // 
            this.dropdownBGtype.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.dropdownBGtype.Items.AddRange(new object[] {
            "Terrain",
            "Heightmap"});
            this.dropdownBGtype.Name = "dropdownBGtype";
            this.dropdownBGtype.Size = new System.Drawing.Size(75, 25);
            this.dropdownBGtype.SelectedIndexChanged += new System.EventHandler(this.dropdownBGtype_SelectedIndexChanged);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
            // 
            // panMap
            // 
            this.panMap.BackColor = System.Drawing.Color.Black;
            this.panMap.Location = new System.Drawing.Point(50, 50);
            this.panMap.Name = "panMap";
            this.panMap.Size = new System.Drawing.Size(139, 127);
            this.panMap.TabIndex = 0;
            this.panMap.Paint += new System.Windows.Forms.PaintEventHandler(this.panMap_Paint);
            this.panMap.MouseDown += new System.Windows.Forms.MouseEventHandler(this.panMap_MouseDown);
            this.panMap.MouseMove += new System.Windows.Forms.MouseEventHandler(this.panMap_MouseMove);
            this.panMap.MouseUp += new System.Windows.Forms.MouseEventHandler(this.panMap_MouseUp);
            // 
            // frmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(746, 502);
            this.Controls.Add(this.splitOuter);
            this.Controls.Add(this.toolStrip1);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "frmMain";
            this.Text = "Chaotic Rage Maptool";
            this.Load += new System.EventHandler(this.frmMain_Load);
            this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.frmMain_KeyUp);
            this.panOuter.ResumeLayout(false);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.splitRight.Panel1.ResumeLayout(false);
            this.splitRight.Panel2.ResumeLayout(false);
            this.splitRight.ResumeLayout(false);
            this.splitOuter.Panel1.ResumeLayout(false);
            this.splitOuter.Panel2.ResumeLayout(false);
            this.splitOuter.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Panel panOuter;
        private Maptool.DoubleBufferPanel panMap;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem mnuFileNew;
        private System.Windows.Forms.ToolStripMenuItem mnuFileOpen;
        private System.Windows.Forms.ToolStripMenuItem mnuFileSave;
        private System.Windows.Forms.ToolStripMenuItem mnuFileSaveas;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem mnuFileExit;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton toolZones;
        private System.Windows.Forms.SplitContainer splitRight;
        private System.Windows.Forms.ListView list;
        private System.Windows.Forms.PropertyGrid grid;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.SplitContainer splitOuter;
        private System.Windows.Forms.ToolStripMenuItem editToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem mnuEditMapsettings;
        private System.Windows.Forms.SaveFileDialog diaSave;
        private System.Windows.Forms.OpenFileDialog diaOpen;
        private System.Windows.Forms.ImageList imgList;
        private System.Windows.Forms.ToolStripButton toolLights;
        private System.Windows.Forms.ToolStripButton toolObjects;
        private System.Windows.Forms.ToolStripComboBox dropdownBGtype;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
    }
}

