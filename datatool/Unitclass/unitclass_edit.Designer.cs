namespace datatool
{
    partial class UnitclassEdit
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(UnitclassEdit));
            this.gridSpew = new System.Windows.Forms.PropertyGrid();
            this.label1 = new System.Windows.Forms.Label();
            this.txtName = new System.Windows.Forms.TextBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.btnOK = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.btnAddSpew = new System.Windows.Forms.Button();
            this.btnRemove = new System.Windows.Forms.Button();
            this.lstSpew = new System.Windows.Forms.ListView();
            this.columnHeader1 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.imgSmallIcons = new System.Windows.Forms.ImageList(this.components);
            this.btnActions = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // gridSpew
            // 
            this.gridSpew.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.gridSpew.Location = new System.Drawing.Point(6, 19);
            this.gridSpew.Name = "gridSpew";
            this.gridSpew.PropertySort = System.Windows.Forms.PropertySort.NoSort;
            this.gridSpew.Size = new System.Drawing.Size(381, 234);
            this.gridSpew.TabIndex = 4;
            this.gridSpew.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.gridSpew_PropertyValueChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 15);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(38, 13);
            this.label1.TabIndex = 5;
            this.label1.Text = "Name:";
            // 
            // txtName
            // 
            this.txtName.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtName.Location = new System.Drawing.Point(79, 12);
            this.txtName.Name = "txtName";
            this.txtName.Size = new System.Drawing.Size(328, 20);
            this.txtName.TabIndex = 6;
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.gridSpew);
            this.groupBox1.Location = new System.Drawing.Point(12, 180);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(395, 259);
            this.groupBox1.TabIndex = 7;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Settings group";
            // 
            // btnOK
            // 
            this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOK.Location = new System.Drawing.Point(325, 461);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(82, 27);
            this.btnOK.TabIndex = 8;
            this.btnOK.Text = "&OK";
            this.btnOK.UseVisualStyleBackColor = true;
            this.btnOK.Click += new System.EventHandler(this.btnSave_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.Location = new System.Drawing.Point(238, 461);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(81, 27);
            this.btnCancel.TabIndex = 9;
            this.btnCancel.Text = "&Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 38);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(48, 13);
            this.label2.TabIndex = 11;
            this.label2.Text = "Settings:";
            // 
            // btnAddSpew
            // 
            this.btnAddSpew.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnAddSpew.Location = new System.Drawing.Point(325, 139);
            this.btnAddSpew.Name = "btnAddSpew";
            this.btnAddSpew.Size = new System.Drawing.Size(82, 26);
            this.btnAddSpew.TabIndex = 12;
            this.btnAddSpew.Text = "&Add";
            this.btnAddSpew.UseVisualStyleBackColor = true;
            this.btnAddSpew.Click += new System.EventHandler(this.btnAddSpew_Click);
            // 
            // btnRemove
            // 
            this.btnRemove.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnRemove.Location = new System.Drawing.Point(238, 139);
            this.btnRemove.Name = "btnRemove";
            this.btnRemove.Size = new System.Drawing.Size(81, 26);
            this.btnRemove.TabIndex = 13;
            this.btnRemove.Text = "&Remove";
            this.btnRemove.UseVisualStyleBackColor = true;
            this.btnRemove.Click += new System.EventHandler(this.btnRemove_Click);
            // 
            // lstSpew
            // 
            this.lstSpew.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.lstSpew.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1});
            this.lstSpew.FullRowSelect = true;
            this.lstSpew.Location = new System.Drawing.Point(79, 38);
            this.lstSpew.Name = "lstSpew";
            this.lstSpew.Size = new System.Drawing.Size(328, 95);
            this.lstSpew.SmallImageList = this.imgSmallIcons;
            this.lstSpew.TabIndex = 14;
            this.lstSpew.UseCompatibleStateImageBehavior = false;
            this.lstSpew.View = System.Windows.Forms.View.Details;
            this.lstSpew.SelectedIndexChanged += new System.EventHandler(this.lstSpew_SelectedIndexChanged);
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "Settings group name";
            this.columnHeader1.Width = 300;
            // 
            // imgSmallIcons
            // 
            this.imgSmallIcons.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imgSmallIcons.ImageStream")));
            this.imgSmallIcons.TransparentColor = System.Drawing.Color.Transparent;
            this.imgSmallIcons.Images.SetKeyName(0, "document-properties.png");
            // 
            // btnActions
            // 
            this.btnActions.Location = new System.Drawing.Point(12, 465);
            this.btnActions.Name = "btnActions";
            this.btnActions.Size = new System.Drawing.Size(82, 23);
            this.btnActions.TabIndex = 15;
            this.btnActions.Text = "Ac&tions";
            this.btnActions.UseVisualStyleBackColor = true;
            this.btnActions.Click += new System.EventHandler(this.btnActions_Click);
            // 
            // UnitclassEdit
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(419, 500);
            this.Controls.Add(this.btnActions);
            this.Controls.Add(this.lstSpew);
            this.Controls.Add(this.btnRemove);
            this.Controls.Add(this.btnAddSpew);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.txtName);
            this.Controls.Add(this.label1);
            this.MinimumSize = new System.Drawing.Size(297, 437);
            this.Name = "UnitclassEdit";
            this.Text = "Edit unit class";
            this.Load += new System.EventHandler(this.Form_Load);
            this.groupBox1.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PropertyGrid gridSpew;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtName;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button btnAddSpew;
        private System.Windows.Forms.Button btnRemove;
        private System.Windows.Forms.ListView lstSpew;
        private System.Windows.Forms.ImageList imgSmallIcons;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.Button btnActions;
    }
}