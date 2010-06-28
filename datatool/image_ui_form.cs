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
        private string preview_filename;

        public image_ui_form()
        {
            InitializeComponent();
        }

        private void image_ui_form_Load(object sender, EventArgs e)
        {
            string[] files;

            this.list.Items.Clear();

            switch (this.directory) {
                case "areatypes":
                    files = System.IO.Directory.GetFiles(Program.datapath + "/areatypes", "*-fr0.bmp");

                    foreach (string f in files) {
                        string g = System.IO.Path.GetFileName(f);
                        g = g.Replace("-fr0.bmp", "");
                        this.list.Items.Add(g);
                    }

                    labAdding.Text = "Copy images into the 'areatypes' directory, named <IMAGE>-fr0.bmp, where <IMAGE> is the image name, which should be in lowercase and without spaces.";

                    this.preview_filename = Program.datapath + "/areatypes/{0}-fr0.bmp";
                    break;

                case "particletypes":
                    files = System.IO.Directory.GetDirectories(Program.datapath + "/particletypes", "*");

                    foreach (string f in files) {
                        string g = System.IO.Path.GetFileName(f);
                        this.list.Items.Add(g);
                    }

                    labAdding.Text = "Create a directory in the 'particletypes' directory, named the same as the image name, which should be in lowercase and without spaces. Files should be named <ANGLE>deg_fr<FRAME>.bmp";
                    this.preview_filename = Program.datapath + "/particletypes/{0}/0deg_fr0.bmp";
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

        private void list_SelectedIndexChanged(object sender, EventArgs e)
        {
            string filename = string.Format(this.preview_filename, (string) this.list.SelectedItem);
            Image img;

            picPreview.Image = null;
            if (picPreview.BackgroundImage != null) {
                picPreview.BackgroundImage.Dispose();
            }

            try {
                img = new Bitmap(filename);
            } catch {
                picPreview.BackgroundImage = null;
                picPreview.Image = Properties.Resources.error_large;
                return;
            }

            picPreview.BackgroundImage = img;

            if (img.Width > picPreview.Width || img.Height > picPreview.Height) {
                picPreview.BackgroundImageLayout = ImageLayout.Zoom;
            } else {
                picPreview.BackgroundImageLayout = ImageLayout.Center;
            }
        }
    }
}
