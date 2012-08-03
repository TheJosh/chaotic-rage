namespace datatool
{
    partial class Main
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
            if (disposing && (components != null))
            {
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Main));
            this.menu = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newModToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openModToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.metricsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.MenuMetricsWeaponsGrid = new System.Windows.Forms.ToolStripMenuItem();
            this.MenuMetricsWeaponsCompare = new System.Windows.Forms.ToolStripMenuItem();
            this.toolbar = new System.Windows.Forms.ToolStrip();
            this.toolWeapons = new System.Windows.Forms.ToolStripButton();
            this.status = new System.Windows.Forms.StatusStrip();
            this.statusDatapath = new System.Windows.Forms.ToolStripStatusLabel();
            this.toolUnitTypes = new System.Windows.Forms.ToolStripButton();
            this.menu.SuspendLayout();
            this.toolbar.SuspendLayout();
            this.status.SuspendLayout();
            this.SuspendLayout();
            // 
            // menu
            // 
            this.menu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.metricsToolStripMenuItem});
            this.menu.Location = new System.Drawing.Point(0, 0);
            this.menu.Name = "menu";
            this.menu.Size = new System.Drawing.Size(766, 24);
            this.menu.TabIndex = 8;
            this.menu.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newModToolStripMenuItem,
            this.openModToolStripMenuItem,
            this.toolStripSeparator2,
            this.saveToolStripMenuItem,
            this.toolStripSeparator3,
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "&File";
            // 
            // newModToolStripMenuItem
            // 
            this.newModToolStripMenuItem.Name = "newModToolStripMenuItem";
            this.newModToolStripMenuItem.Size = new System.Drawing.Size(140, 22);
            this.newModToolStripMenuItem.Text = "New Mod...";
            this.newModToolStripMenuItem.Click += new System.EventHandler(this.newModToolStripMenuItem_Click);
            // 
            // openModToolStripMenuItem
            // 
            this.openModToolStripMenuItem.Name = "openModToolStripMenuItem";
            this.openModToolStripMenuItem.Size = new System.Drawing.Size(140, 22);
            this.openModToolStripMenuItem.Text = "Open Mod...";
            this.openModToolStripMenuItem.Click += new System.EventHandler(this.openModToolStripMenuItem_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(137, 6);
            // 
            // saveToolStripMenuItem
            // 
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.Size = new System.Drawing.Size(140, 22);
            this.saveToolStripMenuItem.Text = "&Save Mod";
            this.saveToolStripMenuItem.Click += new System.EventHandler(this.saveToolStripMenuItem_Click);
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(137, 6);
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(140, 22);
            this.exitToolStripMenuItem.Text = "E&xit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // metricsToolStripMenuItem
            // 
            this.metricsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.MenuMetricsWeaponsGrid,
            this.MenuMetricsWeaponsCompare});
            this.metricsToolStripMenuItem.Name = "metricsToolStripMenuItem";
            this.metricsToolStripMenuItem.Size = new System.Drawing.Size(58, 20);
            this.metricsToolStripMenuItem.Tag = "RequireFile";
            this.metricsToolStripMenuItem.Text = "&Metrics";
            // 
            // MenuMetricsWeaponsGrid
            // 
            this.MenuMetricsWeaponsGrid.Name = "MenuMetricsWeaponsGrid";
            this.MenuMetricsWeaponsGrid.Size = new System.Drawing.Size(175, 22);
            this.MenuMetricsWeaponsGrid.Text = "Weapons Grid";
            this.MenuMetricsWeaponsGrid.Click += new System.EventHandler(this.MenuMetricsWeapons_Click);
            // 
            // MenuMetricsWeaponsCompare
            // 
            this.MenuMetricsWeaponsCompare.Name = "MenuMetricsWeaponsCompare";
            this.MenuMetricsWeaponsCompare.Size = new System.Drawing.Size(175, 22);
            this.MenuMetricsWeaponsCompare.Text = "Weapons Compare";
            this.MenuMetricsWeaponsCompare.Click += new System.EventHandler(this.MenuMetricsWeaponsCompare_Click);
            // 
            // toolbar
            // 
            this.toolbar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolWeapons,
            this.toolUnitTypes});
            this.toolbar.Location = new System.Drawing.Point(0, 24);
            this.toolbar.Name = "toolbar";
            this.toolbar.Size = new System.Drawing.Size(766, 25);
            this.toolbar.TabIndex = 10;
            this.toolbar.Text = "toolStrip1";
            // 
            // toolWeapons
            // 
            this.toolWeapons.Image = ((System.Drawing.Image)(resources.GetObject("toolWeapons.Image")));
            this.toolWeapons.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolWeapons.Name = "toolWeapons";
            this.toolWeapons.Size = new System.Drawing.Size(76, 22);
            this.toolWeapons.Text = "Weapons";
            this.toolWeapons.Click += new System.EventHandler(this.toolWeapons_Click);
            // 
            // status
            // 
            this.status.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.statusDatapath});
            this.status.Location = new System.Drawing.Point(0, 344);
            this.status.Name = "status";
            this.status.Size = new System.Drawing.Size(766, 22);
            this.status.TabIndex = 11;
            // 
            // statusDatapath
            // 
            this.statusDatapath.Name = "statusDatapath";
            this.statusDatapath.Size = new System.Drawing.Size(74, 17);
            this.statusDatapath.Text = "No Datapath";
            this.statusDatapath.ToolTipText = "No datapath";
            // 
            // toolUnitTypes
            // 
            this.toolUnitTypes.Image = ((System.Drawing.Image)(resources.GetObject("toolUnitTypes.Image")));
            this.toolUnitTypes.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolUnitTypes.Name = "toolUnitTypes";
            this.toolUnitTypes.Size = new System.Drawing.Size(83, 22);
            this.toolUnitTypes.Text = "Unit Types";
            this.toolUnitTypes.Click += new System.EventHandler(this.toolStripButton1_Click);
            // 
            // Main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(766, 366);
            this.Controls.Add(this.status);
            this.Controls.Add(this.toolbar);
            this.Controls.Add(this.menu);
            this.IsMdiContainer = true;
            this.MainMenuStrip = this.menu;
            this.Name = "Main";
            this.Text = "Chaotic Rage datatool";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Main_FormClosing);
            this.Load += new System.EventHandler(this.Main_Load);
            this.menu.ResumeLayout(false);
            this.menu.PerformLayout();
            this.toolbar.ResumeLayout(false);
            this.toolbar.PerformLayout();
            this.status.ResumeLayout(false);
            this.status.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menu;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.ToolStrip toolbar;
        private System.Windows.Forms.ToolStripButton toolWeapons;
        private System.Windows.Forms.StatusStrip status;
        private System.Windows.Forms.ToolStripStatusLabel statusDatapath;
        private System.Windows.Forms.ToolStripMenuItem metricsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem MenuMetricsWeaponsGrid;
        private System.Windows.Forms.ToolStripMenuItem MenuMetricsWeaponsCompare;
        private System.Windows.Forms.ToolStripMenuItem newModToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openModToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripButton toolUnitTypes;
    }
}

