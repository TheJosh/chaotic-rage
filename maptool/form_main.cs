using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Maptool
{
    public partial class frmMain : Form
    {
        public frmMain()
        {
            InitializeComponent();
            list.Select();
        }

        private void frmMain_Load(object sender, EventArgs e)
        {
            resetUI();
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
            toolObjects.Checked = false;
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
         * Engages the 'Objects' tool
         **/
        private void toolObjects_Click(object sender, EventArgs e)
        {
            toggleOn(toolObjects);
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
    }
}
