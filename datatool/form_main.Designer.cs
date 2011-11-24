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
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.toolParticles = new System.Windows.Forms.ToolStripButton();
            this.toolWeapons = new System.Windows.Forms.ToolStripButton();
            this.toolUnittypes = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.toolParticleGenerators = new System.Windows.Forms.ToolStripButton();
            this.toolModifiers = new System.Windows.Forms.ToolStripButton();
            this.status = new System.Windows.Forms.StatusStrip();
            this.statusDatapath = new System.Windows.Forms.ToolStripStatusLabel();
            this.menu.SuspendLayout();
            this.toolStrip1.SuspendLayout();
            this.status.SuspendLayout();
            this.SuspendLayout();
            // 
            // menu
            // 
            this.menu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem});
            this.menu.Location = new System.Drawing.Point(0, 0);
            this.menu.Name = "menu";
            this.menu.Size = new System.Drawing.Size(766, 24);
            this.menu.TabIndex = 8;
            this.menu.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.saveToolStripMenuItem,
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(35, 20);
            this.fileToolStripMenuItem.Text = "&File";
            // 
            // saveToolStripMenuItem
            // 
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.saveToolStripMenuItem.Text = "&Save";
            this.saveToolStripMenuItem.Click += new System.EventHandler(this.saveToolStripMenuItem_Click);
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.exitToolStripMenuItem.Text = "E&xit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // toolStrip1
            // 
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolParticles,
            this.toolWeapons,
            this.toolUnittypes,
            this.toolStripSeparator1,
            this.toolParticleGenerators,
            this.toolModifiers});
            this.toolStrip1.Location = new System.Drawing.Point(0, 24);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(766, 25);
            this.toolStrip1.TabIndex = 10;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // toolParticles
            // 
            this.toolParticles.Image = ((System.Drawing.Image)(resources.GetObject("toolParticles.Image")));
            this.toolParticles.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolParticles.Name = "toolParticles";
            this.toolParticles.Size = new System.Drawing.Size(67, 22);
            this.toolParticles.Text = "Particles";
            this.toolParticles.Click += new System.EventHandler(this.toolParticles_Click);
            // 
            // toolWeapons
            // 
            this.toolWeapons.Image = ((System.Drawing.Image)(resources.GetObject("toolWeapons.Image")));
            this.toolWeapons.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolWeapons.Name = "toolWeapons";
            this.toolWeapons.Size = new System.Drawing.Size(72, 22);
            this.toolWeapons.Text = "Weapons";
            this.toolWeapons.Click += new System.EventHandler(this.toolWeapons_Click);
            // 
            // toolUnittypes
            // 
            this.toolUnittypes.Image = ((System.Drawing.Image)(resources.GetObject("toolUnittypes.Image")));
            this.toolUnittypes.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolUnittypes.Name = "toolUnittypes";
            this.toolUnittypes.Size = new System.Drawing.Size(76, 22);
            this.toolUnittypes.Text = "Unit types";
            this.toolUnittypes.Click += new System.EventHandler(this.toolUnittypes_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
            // 
            // toolParticleGenerators
            // 
            this.toolParticleGenerators.Enabled = false;
            this.toolParticleGenerators.Image = ((System.Drawing.Image)(resources.GetObject("toolParticleGenerators.Image")));
            this.toolParticleGenerators.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolParticleGenerators.Name = "toolParticleGenerators";
            this.toolParticleGenerators.Size = new System.Drawing.Size(119, 22);
            this.toolParticleGenerators.Text = "Particle Generators";
            // 
            // toolModifiers
            // 
            this.toolModifiers.Enabled = false;
            this.toolModifiers.Image = ((System.Drawing.Image)(resources.GetObject("toolModifiers.Image")));
            this.toolModifiers.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolModifiers.Name = "toolModifiers";
            this.toolModifiers.Size = new System.Drawing.Size(68, 22);
            this.toolModifiers.Text = "Modifers";
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
            this.statusDatapath.Size = new System.Drawing.Size(68, 17);
            this.statusDatapath.Text = "No Datapath";
            this.statusDatapath.ToolTipText = "No datapath";
            // 
            // Main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(766, 366);
            this.Controls.Add(this.status);
            this.Controls.Add(this.toolStrip1);
            this.Controls.Add(this.menu);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.IsMdiContainer = true;
            this.MainMenuStrip = this.menu;
            this.Name = "Main";
            this.Text = "Chaotic Rage datatool";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.Load += new System.EventHandler(this.Main_Load);
            this.menu.ResumeLayout(false);
            this.menu.PerformLayout();
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
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
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton toolParticles;
        private System.Windows.Forms.ToolStripButton toolWeapons;
        private System.Windows.Forms.ToolStripButton toolUnittypes;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton toolParticleGenerators;
        private System.Windows.Forms.ToolStripButton toolModifiers;
        private System.Windows.Forms.StatusStrip status;
        private System.Windows.Forms.ToolStripStatusLabel statusDatapath;
    }
}

