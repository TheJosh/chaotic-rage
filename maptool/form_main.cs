using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;


namespace Maptool
{
    public partial class frmMain : Form
    {
        private Tool currTool = null;
        private Entity currEntity = null;
        private EntityType currEntityType = null;

        private int downX, downY;
        private List<Entity> entities;



        public frmMain()
        {
            InitializeComponent();
            entities = new List<Entity>();

        }

        private void frmMain_Load(object sender, EventArgs e)
        {
            list.Select();
            resetUI();
            this.WindowState = FormWindowState.Maximized;
            this.KeyPreview = true;
        }


        /**
         * Resets the user interface to the initial defaults
         **/
        private void resetUI()
        {
            panMap.Size = new Size(1, 1);
            panMap.Size = new Size(2000, 2000);

            toggleOn(toolZones);
            setCurrEntity(null);
            setCurrEntityType(null);
            setCurrTool(null);
            panMap.Refresh();
            grid.Refresh();
        }

        /**
         * Toggles one of the tool buttons to be on, turning off the others
         **/
        private void toggleOn(ToolStripButton on)
        {
            toolZones.Checked = false;
            on.Checked = true;
        }


        /**
         * Engages the 'Zones' tool
         **/
        private void toolZones_Click(object sender, EventArgs e)
        {
            toggleOn(toolZones);
            setCurrTool(Tools.Zone);
        }


        /**
         * Blanks the map
         **/
        private void mnuFileNew_Click(object sender, EventArgs e)
        {
            entities.Clear();
            resetUI();
        }

        private void mnuFileOpen_Click(object sender, EventArgs e)
        {
            if (diaOpen.ShowDialog() == System.Windows.Forms.DialogResult.OK) {
                Map m = new Map();
                m.Load(diaOpen.FileName);
                this.entities = m.Entities;
                resetUI();
            }
        }

        private void mnuFileSaveas_Click(object sender, EventArgs e)
        {
            if (diaSave.ShowDialog() == System.Windows.Forms.DialogResult.OK) {
                Map m = new Map();
                m.Entities = this.entities;
                m.Save(diaSave.FileName);
            }
        }

        private void mnuFileExit_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void mnuEditMapsettings_Click(object sender, EventArgs e)
        {
            frmMapSettings f = new frmMapSettings();
            f.ShowDialog();
        }


        /**
         * Select a tool
         **/
        private void setCurrTool(Tool tool)
        {
            currTool = tool;

            setCurrEntity(null);
            setCurrEntityType(null);
            list.Items.Clear();
            imgList.Images.Clear();
            splitRight.Panel2Collapsed = true;

            if (tool == null) return;

            int j = 0;
            foreach (EntityType type in tool.getTypes()) {
                EntityTypeListItem itm = new EntityTypeListItem();
                itm.EntityType = type;
                list.Items.Add(itm);

                if (type.Image != null) {
                    imgList.Images.Add(type.Image);
                    itm.ImageIndex = j++;
                }
            }

            splitRight.Panel2Collapsed = (list.Items.Count < 2);

            if (list.Items.Count > 0) {
                list.Items[0].Selected = true;
                setCurrEntityType(((EntityTypeListItem)list.Items[0]).EntityType);
            }
        }

        /**
         * Select an entity
         **/
        private void setCurrEntity(Entity ent)
        {
            currEntity = ent;
            grid.SelectedObject = ent;
        }

        /**
         * Select an entity type
         **/
        private void setCurrEntityType(EntityType type)
        {
            currEntityType = type;
        }



        /**
         * List selection change
         **/
        private void list_SelectedIndexChanged(object sender, EventArgs e)
        {
            setCurrEntityType(null);
            if (list.SelectedItems.Count > 0) {
                setCurrEntityType(((EntityTypeListItem)list.SelectedItems[0]).EntityType);
            }
        }

        /**
        * Mouse down logic
        **/
        private void panMap_MouseDown(object sender, MouseEventArgs e)
        {
            downX = downY = -1;

            if (e.Button == System.Windows.Forms.MouseButtons.Left) {
                try_select(e.X, e.Y);
                if (currEntity != null) {
                    downX = e.X;
                    downY = e.Y;
                }
            }

            panMap.Focus();
            panMap.Refresh();
            grid.Refresh();
        }

        private void panMap_MouseMove(object sender, MouseEventArgs e)
        {
            if (downX == -1) return;
            if (currEntity == null) return;

            currEntity.X = e.X;
            currEntity.Y = e.Y;

            panMap.Refresh();
            grid.Refresh();
        }

        /**
        * Mouse up logic
        **/
        private void panMap_MouseUp(object sender, MouseEventArgs e)
        {
            downX = downY = -1;

            if (e.Button == System.Windows.Forms.MouseButtons.Left) {
                try_add(e.X, e.Y);
            }

            panMap.Refresh();
            grid.Refresh();
        }


        /**
        * Keyboard up logic
        **/
        private void frmMain_KeyUp(object sender, KeyEventArgs e)
        {
            e.Handled = true;
            switch (e.KeyCode) {
                case Keys.Left:
                    if (currEntity != null) currEntity.X -= (e.Control ? 15 : 1);
                    break;

                case Keys.Right:
                    if (currEntity != null) currEntity.X += (e.Control ? 15 : 1);
                    break;

                case Keys.Up:
                    if (currEntity != null) currEntity.Y -= (e.Control ? 15 : 1);
                    break;

                case Keys.Down:
                    if (currEntity != null) currEntity.Y += (e.Control ? 15 : 1);
                    break;

                case Keys.Escape:
                    setCurrEntity(null);
                    break;

                case Keys.Delete:
                    if (currEntity != null) {
                        entities.Remove(currEntity);
                        setCurrEntity(null);
                    }
                    break;

                default:
                    e.Handled = false;
                    return;
            }

            panMap.Refresh();
            grid.Refresh();
        }


        private void do_paint(Graphics g)
        {
            Brush br_darkgray = new SolidBrush(Color.FromArgb(20, 20, 20));

            // Zones
            foreach (Entity ent in entities) {
                if (!(ent is ZoneEntity)) continue;

                ZoneEntity entc = (ZoneEntity)ent;

                g.DrawRectangle((ent == currEntity ? Pens.Red : Pens.White), entc.X - entc.Width / 2, entc.Y - entc.Height / 2, entc.Width, entc.Height);
            }
        }


        /**
         * Draws everything
         **/
        private void panMap_Paint(object sender, PaintEventArgs e)
        {
            this.do_paint(e.Graphics);
        }

        /**
        * If there is an entity at the current location, select it for editing
        **/
        public void try_select(int mousex, int mousey)
        {
            if (currTool == null) return;

            setCurrEntity(null);

            foreach (Entity e in entities) {
                if (!currTool.isThisOurs(e)) continue;

                double dist = Math.Sqrt(((e.X - mousex) * (e.X - mousex)) + ((e.Y - mousey) * (e.Y - mousey)));
                if (dist < 10) {
                    setCurrEntity(e);
                    return;
                }
            }
        }

        /**
        * If there is nothing selected, do an add
        **/
        private void try_add(int mousex, int mousey)
        {
            if (currTool == null) return;
            if (currEntity != null) return;
            if (currEntityType == null) return;

            Entity ent = currTool.createEntity();
            ent.X = mousex;
            ent.Y = mousey;
            ent.Type = currEntityType;
            entities.Add(ent);
            setCurrEntity(ent);
        }

        private void grid_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            panMap.Refresh();
        }

        private void list_Resize(object sender, EventArgs e)
        {
            list.Columns[0].Width = list.Width - 10;
        }
    }

    class EntityTypeListItem : ListViewItem
    {
        private EntityType type;

        public EntityType EntityType
        {
            get { return type; }
            set {
                this.type = value;
                this.Text = value.Name;
            }
        }
    }

    public class DoubleBufferPanel : Panel
    {
        public DoubleBufferPanel()
        {
            this.SetStyle(
                ControlStyles.DoubleBuffer |
                ControlStyles.UserPaint |
                ControlStyles.AllPaintingInWmPaint |
                ControlStyles.Selectable,
                true
            );
            this.UpdateStyles();
        }
    }
}
