using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;

namespace Maptool
{
    public partial class frmMain : Form
    {
        private Tool currTool = null;
        private Entity currEntity = null;
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
            toggleOn(toolAreas);
        }

        /**
         * Toggles one of the tool buttons to be on, turning off the others
         **/
        private void toggleOn(ToolStripButton on)
        {
            toolAreas.Checked = false;
            toolWalls.Checked = false;
            toolObjects.Checked = false;
            toolParticleGenerators.Checked = false;
            toolZones.Checked = false;
            on.Checked = true;
        }


        /**
         * Engages the 'Areas' tool
         **/
        private void toolAreas_Click(object sender, EventArgs e)
        {
            toggleOn(toolAreas);
        }

        /**
         * Engages the 'Walls' tool
         **/
        private void toolWalls_Click(object sender, EventArgs e)
        {
            toggleOn(toolWalls);
            currTool = new WallTool();
        }

        /**
         * Engages the 'Objects' tool
         **/
        private void toolObjects_Click(object sender, EventArgs e)
        {
            toggleOn(toolObjects);
        }

        /**
         * Engages the 'Particle Generators' tool
         **/
        private void toolParticleGenerators_Click(object sender, EventArgs e)
        {
            toggleOn(toolParticleGenerators);
        }

        /**
         * Engages the 'Zones' tool
         **/
        private void toolZones_Click(object sender, EventArgs e)
        {
            toggleOn(toolZones);
        }


        /**
         * Blanks the map
         **/
        private void mnuFileNew_Click(object sender, EventArgs e)
        {
            resetUI();
        }

        private void mnuEditMapsettings_Click(object sender, EventArgs e)
        {
            frmMapSettings f = new frmMapSettings();
            f.ShowDialog();
        }



        private void setCurrEntity(Entity ent)
        {
            currEntity = ent;
            grid.SelectedObject = ent;
        }


        /**
        * Dispatches various mouse events
        **/
        private void panMap_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == System.Windows.Forms.MouseButtons.Left) {
                add_or_edit(e.X, e.Y);
            }
        }


        /**
        * If there is an entity at the current location, select it for editing
        * Else, just add something new
        **/
        private void add_or_edit(int mousex, int mousey)
        {
            Entity ent;

            if (currTool == null) return;

            currEntity = null;
            foreach (Entity e in entities) {
                if (! currTool.isThisOurs(e)) continue;

                double dist = Math.Sqrt(((e.X - mousex) * (e.X - mousex)) + ((e.Y - mousey) * (e.Y - mousey)));
                if (dist < 10) {
                    setCurrEntity(e);
                    break;
                }
            }

            if (currEntity == null) {
                ent = currTool.createEntity();
                ent.X = mousex;
                ent.Y = mousey;
                entities.Add(ent);
                setCurrEntity(ent);
            }

            panMap.Refresh();
        }


        /**
         * Draws everything
         **/
        private void panMap_Paint(object sender, PaintEventArgs e)
        {
            Graphics g = e.Graphics;

            foreach (Entity ent in entities) {
                if (ent == currEntity) {
                    g.DrawEllipse(Pens.Red, ent.X - 5, ent.Y - 5, 10, 10);
                } else {
                    g.DrawEllipse(Pens.White, ent.X - 5, ent.Y - 5, 10, 10);
                }
            }
        }

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
                    break;
            }

            panMap.Refresh();
            grid.Refresh();
        }
    }
}
