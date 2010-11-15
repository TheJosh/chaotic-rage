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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmMain));
            this.panOuter = new System.Windows.Forms.Panel();
            this.panMap = new System.Windows.Forms.Panel();
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
            this.toolAreas = new System.Windows.Forms.ToolStripButton();
            this.toolWalls = new System.Windows.Forms.ToolStripButton();
            this.toolObjects = new System.Windows.Forms.ToolStripButton();
            this.toolParticleGenerators = new System.Windows.Forms.ToolStripButton();
            this.toolZones = new System.Windows.Forms.ToolStripButton();
            this.splitRight = new System.Windows.Forms.SplitContainer();
            this.list = new System.Windows.Forms.ListView();
            this.columnHeader1 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.grid = new System.Windows.Forms.PropertyGrid();
            this.splitOuter = new System.Windows.Forms.SplitContainer();
            this.panOuter.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            this.toolStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitRight)).BeginInit();
            this.splitRight.Panel1.SuspendLayout();
            this.splitRight.Panel2.SuspendLayout();
            this.splitRight.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitOuter)).BeginInit();
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
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(35, 20);
            this.fileToolStripMenuItem.Text = "&File";
            // 
            // mnuFileNew
            // 
            this.mnuFileNew.Name = "mnuFileNew";
            this.mnuFileNew.Size = new System.Drawing.Size(136, 22);
            this.mnuFileNew.Text = "&New";
            this.mnuFileNew.Click += new System.EventHandler(this.mnuFileNew_Click);
            // 
            // mnuFileOpen
            // 
            this.mnuFileOpen.Name = "mnuFileOpen";
            this.mnuFileOpen.Size = new System.Drawing.Size(136, 22);
            this.mnuFileOpen.Text = "&Open...";
            // 
            // mnuFileSave
            // 
            this.mnuFileSave.Name = "mnuFileSave";
            this.mnuFileSave.Size = new System.Drawing.Size(136, 22);
            this.mnuFileSave.Text = "&Save";
            // 
            // mnuFileSaveas
            // 
            this.mnuFileSaveas.Name = "mnuFileSaveas";
            this.mnuFileSaveas.Size = new System.Drawing.Size(136, 22);
            this.mnuFileSaveas.Text = "&Save As...";
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(133, 6);
            // 
            // mnuFileExit
            // 
            this.mnuFileExit.Name = "mnuFileExit";
            this.mnuFileExit.Size = new System.Drawing.Size(136, 22);
            this.mnuFileExit.Text = "E&xit";
            // 
            // editToolStripMenuItem
            // 
            this.editToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuEditMapsettings});
            this.editToolStripMenuItem.Name = "editToolStripMenuItem";
            this.editToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.editToolStripMenuItem.Text = "&Edit";
            // 
            // mnuEditMapsettings
            // 
            this.mnuEditMapsettings.Name = "mnuEditMapsettings";
            this.mnuEditMapsettings.Size = new System.Drawing.Size(147, 22);
            this.mnuEditMapsettings.Text = "Map &Settings";
            this.mnuEditMapsettings.Click += new System.EventHandler(this.mnuEditMapsettings_Click);
            // 
            // toolStrip1
            // 
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolAreas,
            this.toolWalls,
            this.toolObjects,
            this.toolParticleGenerators,
            this.toolZones});
            this.toolStrip1.Location = new System.Drawing.Point(0, 24);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(746, 25);
            this.toolStrip1.TabIndex = 1;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // toolAreas
            // 
            this.toolAreas.Checked = true;
            this.toolAreas.CheckState = System.Windows.Forms.CheckState.Checked;
            this.toolAreas.Image = global::Maptool.Properties.Resources.areas;
            this.toolAreas.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolAreas.Name = "toolAreas";
            this.toolAreas.Size = new System.Drawing.Size(55, 22);
            this.toolAreas.Text = "Areas";
            this.toolAreas.Click += new System.EventHandler(this.toolAreas_Click);
            // 
            // toolWalls
            // 
            this.toolWalls.Image = ((System.Drawing.Image)(resources.GetObject("toolWalls.Image")));
            this.toolWalls.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolWalls.Name = "toolWalls";
            this.toolWalls.Size = new System.Drawing.Size(52, 22);
            this.toolWalls.Text = "Walls";
            this.toolWalls.Click += new System.EventHandler(this.toolWalls_Click);
            // 
            // toolObjects
            // 
            this.toolObjects.Image = global::Maptool.Properties.Resources.objects;
            this.toolObjects.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolObjects.Name = "toolObjects";
            this.toolObjects.Size = new System.Drawing.Size(64, 22);
            this.toolObjects.Text = "Objects";
            this.toolObjects.Click += new System.EventHandler(this.toolObjects_Click);
            // 
            // toolParticleGenerators
            // 
            this.toolParticleGenerators.Image = ((System.Drawing.Image)(resources.GetObject("toolParticleGenerators.Image")));
            this.toolParticleGenerators.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolParticleGenerators.Name = "toolParticleGenerators";
            this.toolParticleGenerators.Size = new System.Drawing.Size(119, 22);
            this.toolParticleGenerators.Text = "Particle Generators";
            this.toolParticleGenerators.Click += new System.EventHandler(this.toolParticleGenerators_Click);
            // 
            // toolZones
            // 
            this.toolZones.Image = global::Maptool.Properties.Resources.zones;
            this.toolZones.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolZones.Name = "toolZones";
            this.toolZones.Size = new System.Drawing.Size(56, 22);
            this.toolZones.Text = "Zones";
            this.toolZones.Click += new System.EventHandler(this.toolZones_Click);
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
            this.splitRight.Panel1.Controls.Add(this.list);
            // 
            // splitRight.Panel2
            // 
            this.splitRight.Panel2.AutoScroll = true;
            this.splitRight.Panel2.Controls.Add(this.grid);
            this.splitRight.Size = new System.Drawing.Size(179, 453);
            this.splitRight.SplitterDistance = 238;
            this.splitRight.TabIndex = 5;
            // 
            // list
            // 
            this.list.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1});
            this.list.Dock = System.Windows.Forms.DockStyle.Fill;
            this.list.Location = new System.Drawing.Point(0, 0);
            this.list.MultiSelect = false;
            this.list.Name = "list";
            this.list.Size = new System.Drawing.Size(179, 238);
            this.list.TabIndex = 0;
            this.list.UseCompatibleStateImageBehavior = false;
            this.list.View = System.Windows.Forms.View.Details;
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "Name";
            this.columnHeader1.Width = 184;
            // 
            // grid
            // 
            this.grid.Dock = System.Windows.Forms.DockStyle.Fill;
            this.grid.Location = new System.Drawing.Point(0, 0);
            this.grid.Name = "grid";
            this.grid.PropertySort = System.Windows.Forms.PropertySort.Alphabetical;
            this.grid.Size = new System.Drawing.Size(179, 211);
            this.grid.TabIndex = 0;
            this.grid.ToolbarVisible = false;
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
            ((System.ComponentModel.ISupportInitialize)(this.splitRight)).EndInit();
            this.splitRight.ResumeLayout(false);
            this.splitOuter.Panel1.ResumeLayout(false);
            this.splitOuter.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitOuter)).EndInit();
            this.splitOuter.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Panel panOuter;
        private System.Windows.Forms.Panel panMap;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem mnuFileNew;
        private System.Windows.Forms.ToolStripMenuItem mnuFileOpen;
        private System.Windows.Forms.ToolStripMenuItem mnuFileSave;
        private System.Windows.Forms.ToolStripMenuItem mnuFileSaveas;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem mnuFileExit;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton toolAreas;
        private System.Windows.Forms.ToolStripButton toolObjects;
        private System.Windows.Forms.ToolStripButton toolZones;
        private System.Windows.Forms.SplitContainer splitRight;
        private System.Windows.Forms.ListView list;
        private System.Windows.Forms.PropertyGrid grid;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.SplitContainer splitOuter;
        private System.Windows.Forms.ToolStripButton toolParticleGenerators;
        private System.Windows.Forms.ToolStripMenuItem editToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem mnuEditMapsettings;
        private System.Windows.Forms.ToolStripButton toolWalls;
    }
}

