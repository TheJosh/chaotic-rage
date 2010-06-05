using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace datatool
{
    public partial class image_ui_form : Form
    {
        public string directory;
        public string value;

        public image_ui_form()
        {
            InitializeComponent();
        }

        private void image_ui_form_Load(object sender, EventArgs e)
        {
            this.list.Items.Clear();

            switch (this.directory) {
                case "areatypes":
                    string[] files = System.IO.Directory.GetFiles(Program.datapath + "/areatypes", "*-fr0.bmp");

                    foreach (string f in files) {
                        string g = System.IO.Path.GetFileName(f);
                        g = g.Replace("-fr0.bmp", "");
                        this.list.Items.Add(g);
                    }

                    labAdding.Text = "Copy images into the 'areatypes' directory, named IMAGE-fr0.bmp, where IMAGE is the image name, which should be in lowercase and without spaces.";
                    break;

                default:
                    this.DialogResult = System.Windows.Forms.DialogResult.Cancel;
                    this.Close();
                    break;
            }


        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            if (this.list.SelectedItem == null) return;

            this.value = (string) this.list.SelectedItem;
            this.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.Close();
        }
    }
}
