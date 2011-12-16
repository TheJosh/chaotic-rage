namespace datatool
{
    partial class metric_compare
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
            this.datagrid = new System.Windows.Forms.DataGridView();
            this.metriccombo = new System.Windows.Forms.ComboBox();
            ((System.ComponentModel.ISupportInitialize)(this.datagrid)).BeginInit();
            this.SuspendLayout();
            // 
            // datagrid
            // 
            this.datagrid.AllowUserToAddRows = false;
            this.datagrid.AllowUserToDeleteRows = false;
            this.datagrid.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.datagrid.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.datagrid.Location = new System.Drawing.Point(12, 39);
            this.datagrid.Name = "datagrid";
            this.datagrid.Size = new System.Drawing.Size(681, 430);
            this.datagrid.TabIndex = 1;
            // 
            // metriccombo
            // 
            this.metriccombo.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.metriccombo.FormattingEnabled = true;
            this.metriccombo.Location = new System.Drawing.Point(12, 12);
            this.metriccombo.Name = "metriccombo";
            this.metriccombo.Size = new System.Drawing.Size(681, 21);
            this.metriccombo.TabIndex = 2;
            this.metriccombo.SelectedIndexChanged += new System.EventHandler(this.metriccombo_SelectedIndexChanged);
            // 
            // metric_compare
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(705, 481);
            this.Controls.Add(this.metriccombo);
            this.Controls.Add(this.datagrid);
            this.Name = "metric_compare";
            this.Text = "Metric Compare";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.metric_grid_FormClosed);
            this.Load += new System.EventHandler(this.metric_grid_Load);
            ((System.ComponentModel.ISupportInitialize)(this.datagrid)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.DataGridView datagrid;
        private System.Windows.Forms.ComboBox metriccombo;
    }
}