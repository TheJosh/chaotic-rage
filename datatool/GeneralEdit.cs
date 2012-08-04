using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using SharpGL;

namespace datatool
{
    public partial class GeneralEdit : Form
    {
        private List<base_item> items;
        private string title;
        private Type type;
        private OpenGLControl glPreviewCtl;
        private float rotation;

        public GeneralEdit(List<base_item> items, string title, Type type)
        {
            InitializeComponent();
            this.items = items;
            this.title = title;
            this.type = type;
            this.initPreview();
        }

        private void initPreview() {
            glPreviewCtl = new OpenGLControl();

            ((System.ComponentModel.ISupportInitialize)(this.glPreviewCtl)).BeginInit();

            glPreviewCtl.Dock = DockStyle.Fill;
            glPreviewCtl.BitDepth = 24;
            glPreviewCtl.Resized += new System.EventHandler(this.preview_Resized);
            glPreviewCtl.OpenGLDraw += new System.Windows.Forms.PaintEventHandler(this.preview_OpenGLDraw);
            glPreviewCtl.RenderContextType = RenderContextType.FBO;
            glPreviewCtl.FrameRate = 10;
            panPreview.Controls.Add(glPreviewCtl);

            ((System.ComponentModel.ISupportInitialize)(this.glPreviewCtl)).EndInit();
        }

        private void preview_Resized(object sender, EventArgs e)
        {
            OpenGL gl = glPreviewCtl.OpenGL;

            gl.MatrixMode(OpenGL.GL_PROJECTION);
            gl.LoadIdentity();
            gl.Perspective(60.0f, (double)glPreviewCtl.Width / (double)glPreviewCtl.Height, 0.01, 100.0);
            gl.LookAt(-5, 5, -5, 0, 0, 0, 0, 1, 0);
            gl.MatrixMode(OpenGL.GL_MODELVIEW);
        }

        private void nukePreview()
        {
            panPreview.Controls[0].Dispose();
        }

        private void GeneralEdit_Load(object sender, EventArgs e)
        {
            this.Text = this.title;
            list.Clear();
            foreach (base_item i in items) {
                list.Items.Add(new ExtraListItem(i));
            }
        }

        private void list_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (list.SelectedItems.Count == 0) {
                grid.SelectedObject = null;
                this.Text = this.title;
                return;
            }

            ExtraListItem listitem = (ExtraListItem)list.SelectedItems[0];
            grid.SelectedObject = listitem.Extra;
            this.Text = this.title + " - " + listitem.Extra.getName();
        }

        private void btnAdd_Click(object sender, EventArgs e)
        {
            base_item item = (base_item) type.GetConstructor(new Type[0]).Invoke(new Object[0]);

            ExtraListItem listitem = new ExtraListItem(item);
            items.Add(item);
            list.Items.Add(listitem);
            listitem.Selected = true;
            grid.Focus();
        }

        private void grid_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            if (list.SelectedItems.Count == 0) {
                return;
            }

            ExtraListItem listitem = (ExtraListItem) list.SelectedItems[0];
            listitem.Text = listitem.Extra.getName();
            this.Text = this.title + " - " + listitem.Extra.getName();
        }

        private void btnRemove_Click(object sender, EventArgs e)
        {
            if (list.SelectedItems.Count == 0) {
                return;
            }

            ExtraListItem listitem = (ExtraListItem)list.SelectedItems[0];
            list.SelectedItems.Clear();

            items.Remove(listitem.Extra);
            list.Items.Remove(listitem);
        }

        private void toolPreview_Click(object sender, EventArgs e)
        {
            if (toolPreview.Checked) {
                this.initPreview();
            } else {
                this.nukePreview();
            }
            panPreview.Visible = toolPreview.Checked;
        }

        private void preview_OpenGLDraw(object sender, EventArgs e)
        {
            OpenGL gl = glPreviewCtl.OpenGL;
            
            gl.Clear(OpenGL.GL_COLOR_BUFFER_BIT | OpenGL.GL_DEPTH_BUFFER_BIT);
            gl.LoadIdentity();
            gl.Rotate(rotation, 0.0f, 1.0f, 0.0f);

            //  Draw a coloured pyramid.
            gl.Begin(OpenGL.GL_TRIANGLES);
            gl.Color(1.0f, 0.0f, 0.0f);
            gl.Vertex(0.0f, 1.0f, 0.0f);
            gl.Color(0.0f, 1.0f, 0.0f);
            gl.Vertex(-1.0f, -1.0f, 1.0f);
            gl.Color(0.0f, 0.0f, 1.0f);
            gl.Vertex(1.0f, -1.0f, 1.0f);
            gl.Color(1.0f, 0.0f, 0.0f);
            gl.Vertex(0.0f, 1.0f, 0.0f);
            gl.Color(0.0f, 0.0f, 1.0f);
            gl.Vertex(1.0f, -1.0f, 1.0f);
            gl.Color(0.0f, 1.0f, 0.0f);
            gl.Vertex(1.0f, -1.0f, -1.0f);
            gl.Color(1.0f, 0.0f, 0.0f);
            gl.Vertex(0.0f, 1.0f, 0.0f);
            gl.Color(0.0f, 1.0f, 0.0f);
            gl.Vertex(1.0f, -1.0f, -1.0f);
            gl.Color(0.0f, 0.0f, 1.0f);
            gl.Vertex(-1.0f, -1.0f, -1.0f);
            gl.Color(1.0f, 0.0f, 0.0f);
            gl.Vertex(0.0f, 1.0f, 0.0f);
            gl.Color(0.0f, 0.0f, 1.0f);
            gl.Vertex(-1.0f, -1.0f, -1.0f);
            gl.Color(0.0f, 1.0f, 0.0f);
            gl.Vertex(-1.0f, -1.0f, 1.0f);
            gl.End();

            rotation += 0.3f;
        }
    }
}
