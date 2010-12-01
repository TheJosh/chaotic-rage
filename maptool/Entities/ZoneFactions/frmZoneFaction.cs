using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Maptool
{
    public partial class frmZoneFaction : Form
    {
        public ZoneFactions Value;

        public frmZoneFaction()
        {
            InitializeComponent();
        }

        private void frmZoneFaction_Load(object sender, EventArgs e)
        {
            foreach (Control ctl in this.Controls) {
                if (!(ctl is CheckBox)) continue;
                int idx = int.Parse((string) ((CheckBox)ctl).Tag);
                ((CheckBox)ctl).Checked = Value.getValue((Faction) idx); 
            }
        }

        private void cmdOk_Click(object sender, EventArgs e)
        {
            foreach (Control ctl in this.Controls) {
                if (!(ctl is CheckBox)) continue;
                int idx = int.Parse((string)((CheckBox)ctl).Tag);
                Value.setValue((Faction)idx, ((CheckBox)ctl).Checked);
            }

            this.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.Close();
        }

        private void cmdCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.Close();
        }
    }
}
