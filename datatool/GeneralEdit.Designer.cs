namespace datatool
{
    partial class GeneralEdit
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(GeneralEdit));
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.list = new System.Windows.Forms.ListView();
            this.colName = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.grid = new System.Windows.Forms.PropertyGrid();
            this.tools = new System.Windows.Forms.ToolStrip();
            this.btnAdd = new System.Windows.Forms.ToolStripButton();
            this.btnRemove = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.toolPreview = new System.Windows.Forms.ToolStripSplitButton();
            this.previewOff = new System.Windows.Forms.ToolStripMenuItem();
            this.previewWireframe = new System.Windows.Forms.ToolStripMenuItem();
            this.previewSolid = new System.Windows.Forms.ToolStripMenuItem();
            this.previewTextured = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripContainer1 = new System.Windows.Forms.ToolStripContainer();
            this.panPreview = new System.Windows.Forms.Panel();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.tools.SuspendLayout();
            this.toolStripContainer1.ContentPanel.SuspendLayout();
            this.toolStripContainer1.TopToolStripPanel.SuspendLayout();
            this.toolStripContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.list);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.grid);
            this.splitContainer1.Size = new System.Drawing.Size(565, 434);
            this.splitContainer1.SplitterDistance = 188;
            this.splitContainer1.TabIndex = 0;
            // 
            // list
            // 
            this.list.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.colName});
            this.list.Dock = System.Windows.Forms.DockStyle.Fill;
            this.list.FullRowSelect = true;
            this.list.HideSelection = false;
            this.list.LabelWrap = false;
            this.list.Location = new System.Drawing.Point(0, 0);
            this.list.MultiSelect = false;
            this.list.Name = "list";
            this.list.Size = new System.Drawing.Size(188, 434);
            this.list.TabIndex = 0;
            this.list.UseCompatibleStateImageBehavior = false;
            this.list.View = System.Windows.Forms.View.List;
            this.list.SelectedIndexChanged += new System.EventHandler(this.list_SelectedIndexChanged);
            // 
            // colName
            // 
            this.colName.Text = "Name";
            // 
            // grid
            // 
            this.grid.Dock = System.Windows.Forms.DockStyle.Fill;
            this.grid.Location = new System.Drawing.Point(0, 0);
            this.grid.Name = "grid";
            this.grid.PropertySort = System.Windows.Forms.PropertySort.NoSort;
            this.grid.Size = new System.Drawing.Size(373, 434);
            this.grid.TabIndex = 0;
            this.grid.ToolbarVisible = false;
            this.grid.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.grid_PropertyValueChanged);
            // 
            // tools
            // 
            this.tools.AllowMerge = false;
            this.tools.Dock = System.Windows.Forms.DockStyle.None;
            this.tools.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btnAdd,
            this.btnRemove,
            this.toolStripSeparator1,
            this.toolPreview});
            this.tools.Location = new System.Drawing.Point(3, 0);
            this.tools.Name = "tools";
            this.tools.Size = new System.Drawing.Size(217, 25);
            this.tools.TabIndex = 1;
            this.tools.Text = "toolStrip1";
            // 
            // btnAdd
            // 
            this.btnAdd.Image = global::datatool.Properties.Resources.add;
            this.btnAdd.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnAdd.Name = "btnAdd";
            this.btnAdd.Size = new System.Drawing.Size(49, 22);
            this.btnAdd.Text = "Add";
            this.btnAdd.Click += new System.EventHandler(this.btnAdd_Click);
            // 
            // btnRemove
            // 
            this.btnRemove.Image = ((System.Drawing.Image)(resources.GetObject("btnRemove.Image")));
            this.btnRemove.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnRemove.Name = "btnRemove";
            this.btnRemove.Size = new System.Drawing.Size(70, 22);
            this.btnRemove.Text = "Remove";
            this.btnRemove.Click += new System.EventHandler(this.btnRemove_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
            // 
            // toolPreview
            // 
            this.toolPreview.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.previewOff,
            this.previewWireframe,
            this.previewSolid,
            this.previewTextured});
            this.toolPreview.Image = ((System.Drawing.Image)(resources.GetObject("toolPreview.Image")));
            this.toolPreview.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolPreview.Name = "toolPreview";
            this.toolPreview.Size = new System.Drawing.Size(80, 22);
            this.toolPreview.Text = "Preview";
            // 
            // previewOff
            // 
            this.previewOff.CheckOnClick = true;
            this.previewOff.Name = "previewOff";
            this.previewOff.Size = new System.Drawing.Size(129, 22);
            this.previewOff.Text = "Off";
            this.previewOff.Click += new System.EventHandler(this.previewOff_Click);
            // 
            // previewWireframe
            // 
            this.previewWireframe.CheckOnClick = true;
            this.previewWireframe.Name = "previewWireframe";
            this.previewWireframe.Size = new System.Drawing.Size(129, 22);
            this.previewWireframe.Text = "Wireframe";
            this.previewWireframe.Click += new System.EventHandler(this.previewWireframe_Click);
            // 
            // previewSolid
            // 
            this.previewSolid.CheckOnClick = true;
            this.previewSolid.Name = "previewSolid";
            this.previewSolid.Size = new System.Drawing.Size(129, 22);
            this.previewSolid.Text = "Solid";
            this.previewSolid.Click += new System.EventHandler(this.previewSolid_Click);
            // 
            // previewTextured
            // 
            this.previewTextured.Checked = true;
            this.previewTextured.CheckOnClick = true;
            this.previewTextured.CheckState = System.Windows.Forms.CheckState.Checked;
            this.previewTextured.Name = "previewTextured";
            this.previewTextured.Size = new System.Drawing.Size(129, 22);
            this.previewTextured.Text = "Textured";
            this.previewTextured.Click += new System.EventHandler(this.previewTextured_Click);
            // 
            // toolStripContainer1
            // 
            // 
            // toolStripContainer1.ContentPanel
            // 
            this.toolStripContainer1.ContentPanel.AutoScroll = true;
            this.toolStripContainer1.ContentPanel.Controls.Add(this.splitContainer1);
            this.toolStripContainer1.ContentPanel.Controls.Add(this.panPreview);
            this.toolStripContainer1.ContentPanel.Size = new System.Drawing.Size(765, 434);
            this.toolStripContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.toolStripContainer1.Location = new System.Drawing.Point(0, 0);
            this.toolStripContainer1.Name = "toolStripContainer1";
            this.toolStripContainer1.Size = new System.Drawing.Size(765, 459);
            this.toolStripContainer1.TabIndex = 2;
            this.toolStripContainer1.Text = "toolStripContainer1";
            // 
            // toolStripContainer1.TopToolStripPanel
            // 
            this.toolStripContainer1.TopToolStripPanel.Controls.Add(this.tools);
            // 
            // panPreview
            // 
            this.panPreview.Dock = System.Windows.Forms.DockStyle.Right;
            this.panPreview.Location = new System.Drawing.Point(565, 0);
            this.panPreview.Name = "panPreview";
            this.panPreview.Size = new System.Drawing.Size(200, 434);
            this.panPreview.TabIndex = 3;
            // 
            // GeneralEdit
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(765, 459);
            this.Controls.Add(this.toolStripContainer1);
            this.Name = "GeneralEdit";
            this.Text = "GeneralEdit";
            this.Load += new System.EventHandler(this.GeneralEdit_Load);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.tools.ResumeLayout(false);
            this.tools.PerformLayout();
            this.toolStripContainer1.ContentPanel.ResumeLayout(false);
            this.toolStripContainer1.TopToolStripPanel.ResumeLayout(false);
            this.toolStripContainer1.TopToolStripPanel.PerformLayout();
            this.toolStripContainer1.ResumeLayout(false);
            this.toolStripContainer1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.ListView list;
        private System.Windows.Forms.PropertyGrid grid;
        private System.Windows.Forms.ToolStrip tools;
        private System.Windows.Forms.ToolStripButton btnAdd;
        private System.Windows.Forms.ToolStripButton btnRemove;
        private System.Windows.Forms.ToolStripContainer toolStripContainer1;
        private System.Windows.Forms.ColumnHeader colName;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.Panel panPreview;
        private System.Windows.Forms.ToolStripSplitButton toolPreview;
        private System.Windows.Forms.ToolStripMenuItem previewOff;
        private System.Windows.Forms.ToolStripMenuItem previewWireframe;
        private System.Windows.Forms.ToolStripMenuItem previewSolid;
        private System.Windows.Forms.ToolStripMenuItem previewTextured;

    }
}