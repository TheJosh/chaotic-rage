using System;
using System.Collections.Generic;
using System.ComponentModel;

using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace datatool
{
    public partial class range_ui_form : Form
    {
        public range value;

        public range_ui_form()
        {
            InitializeComponent();
        }

        private void range_ui_form_Load(object sender, EventArgs e)
        {
            this.txtMin.Text = value.min.ToString();
            this.txtMax.Text = value.max.ToString();
        }

        private void btnOk_Click(object sender, EventArgs e)
        {
            try {
                this.value.min = int.Parse(this.txtMin.Text);
                this.value.max = int.Parse(this.txtMax.Text);
            } catch {
                MessageBox.Show("Not a number");
                return;
            }

            this.DialogResult = DialogResult.OK;
            this.Close();
        }
    }
}
