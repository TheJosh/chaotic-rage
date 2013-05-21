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
        private Bitmap currPaint = null;
        private Entity currEntity = null;
        private EntityType currEntityType = null;
        private Paint paintTool = null;
        private int zoom = 8;

        private int downX, downY;

        private Map map;


        public frmMain()
        {
            InitializeComponent();
            map = new Map();
            this.dropdownBGtype.SelectedIndex = 0;
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
            panMap.Size = new Size(map.Width * this.zoom, map.Height * this.zoom);

            paintTool = new Paint();
            toolPaint.Tag = 'T';

            setCurrEntity(null);
            setCurrEntityType(null);
            setCurrTool(null);
            setPaintMode(null);
            panMap.Refresh();
            grid.Refresh();

            toggleOn(toolZones);
            setCurrTool(Tools.Zone);
        }

        /**
         * Toggles one of the tool buttons to be on, turning off the others
         **/
        private void toggleOn(ToolStripButton on)
        {
            toolZones.Checked = false;
            toolLights.Checked = false;
            toolObjects.Checked = false;

            if (on != null) on.Checked = true;
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
        * Engages the 'Lights' tool
        **/
        private void toolLights_Click(object sender, EventArgs e)
        {
            toggleOn(toolLights);
            setCurrTool(Tools.Light);
        }


        /**
        * Engages the 'Objects' tools
        **/
        private void toolObjects_Click(object sender, EventArgs e)
        {
            toggleOn(toolObjects);
            setCurrTool(Tools.Object);
        }

        /**
        * Painting the terrain
        **/
        private void paintTerrain_Click(object sender, EventArgs e)
        {
            this.setPaintMode(map.Terrain);
            toolPaint.Tag = 'T';
            toolPaint.Text = "Paint Terrain";
            paintTerrain.Checked = true;
        }

        /**
         * Painting the heightmap
         **/
        private void paintHeightmap_Click(object sender, EventArgs e)
        {
            this.setPaintMode(map.Heightmap);
            toolPaint.Tag = 'H';
            toolPaint.Text = "Paint Heightmap";
            paintHeightmap.Checked = true;
        }


        private void paint_ButtonClick(object sender, EventArgs e)
        {
            if ((char)toolPaint.Tag == 'T') {
                paintTerrain_Click(sender, e);
            } else if ((char)toolPaint.Tag == 'H') {
                paintHeightmap_Click(sender, e);
            }
        }

        /**
         * Blanks the map
         **/
        private void mnuFileNew_Click(object sender, EventArgs e)
        {
            map.Entities.Clear();
            resetUI();
        }

        private void mnuFileOpen_Click(object sender, EventArgs e)
        {
            if (diaOpen.ShowDialog() == System.Windows.Forms.DialogResult.OK) {
                map = new Map();
                map.Load(diaOpen.FileName);
                resetUI();
            }
        }

        private void mnuFileSaveas_Click(object sender, EventArgs e)
        {
            if (diaSave.ShowDialog() == System.Windows.Forms.DialogResult.OK) {
                map.Save(diaSave.FileName);
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

            setPaintMode(null);

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
         * Enables paint mode
         **/
        private void setPaintMode(Bitmap target)
        {
            this.currPaint = target;
            this.paintTool.Target = target;

            if (target == null) return;

            toggleOn(null);
            setCurrTool(null);
            grid.SelectedObject = this.paintTool;
            splitRight.Panel2Collapsed = true;

            paintTerrain.Checked = false;
            paintHeightmap.Checked = false;
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

            if (currTool != null) {
                if (e.Button == System.Windows.Forms.MouseButtons.Left) {
                    try_select(e.X, e.Y);
                    if (currEntity != null) {
                        downX = e.X;
                        downY = e.Y;
                    }
                }
                panMap.Refresh();
                grid.Refresh();

            } else if (currPaint != null) {
                if (e.Button == System.Windows.Forms.MouseButtons.Left) {
                    this.paintTool.mouseDown(e.X / this.zoom, e.Y / this.zoom);
                }
                panMap.Refresh();
                grid.Refresh();
            }
        }

        /**
        * Mouse moves
        **/
        private void panMap_MouseMove(object sender, MouseEventArgs e)
        {
            if (currTool != null) {
                if (downX == -1) return;
                if (currEntity == null) return;

                currEntity.X = e.X / this.zoom;
                currEntity.Y = e.Y / this.zoom;
                panMap.Refresh();
                grid.Refresh();

            } else if (currPaint != null) {
                this.paintTool.mouseMove(e.X / this.zoom, e.Y / this.zoom);
                panMap.Refresh();
                grid.Refresh();
            }
        }

        /**
        * Mouse up logic
        **/
        private void panMap_MouseUp(object sender, MouseEventArgs e)
        {
            if (currTool != null) {
                downX = downY = -1;

                if (e.Button == System.Windows.Forms.MouseButtons.Left) {
                    try_add(e.X, e.Y);
                }

                panMap.Refresh();
                grid.Refresh();

            } else if (currPaint != null) {
                this.paintTool.mouseUp(e.X / this.zoom, e.Y / this.zoom);
                panMap.Refresh();
                grid.Refresh();
            }
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
                        map.Entities.Remove(currEntity);
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

            // Background img
            if (this.dropdownBGtype.SelectedIndex == 0 && map.Terrain != null) {
                g.DrawImage(map.Terrain, 0, 0, map.Width * this.zoom, map.Height * this.zoom);
            } else if (this.dropdownBGtype.SelectedIndex == 1 && map.Terrain != null) {
                g.DrawImage(map.Heightmap, 0, 0, map.Width * this.zoom, map.Height * this.zoom);
            }

            // Zones
            foreach (Entity ent in map.Entities) {
                if (!(ent is ZoneEntity)) continue;

                ZoneEntity entc = (ZoneEntity)ent;

                g.DrawRectangle((ent == currEntity ? Pens.Red : Pens.White), entc.X * this.zoom - entc.Width * this.zoom / 2, entc.Y * this.zoom - entc.Height * this.zoom / 2, entc.Width * this.zoom, entc.Height * this.zoom);
            }

            // Lights
            foreach (Entity ent in map.Entities) {
                if (!(ent is LightEntity)) continue;

                LightEntity entc = (LightEntity)ent;

                g.DrawEllipse((ent == currEntity ? Pens.Red : Pens.White), entc.X * this.zoom, entc.Y * this.zoom, 10, 10);
            }

            // Objects
            foreach (Entity ent in map.Entities) {
                if (!(ent is ObjectEntity)) continue;

                ObjectEntity entc = (ObjectEntity)ent;

                g.DrawRectangle((ent == currEntity ? Pens.Red : Pens.White), entc.X * this.zoom - 5, entc.Y * this.zoom - 5, 10, 10);
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

            mousex /= this.zoom;
            mousey /= this.zoom;

            foreach (Entity e in map.Entities) {
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
            ent.X = mousex / this.zoom;
            ent.Y = mousey / this.zoom;
            ent.Type = currEntityType;
            map.Entities.Add(ent);
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

        private void dropdownBGtype_SelectedIndexChanged(object sender, EventArgs e)
        {
            panMap.Refresh();
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
