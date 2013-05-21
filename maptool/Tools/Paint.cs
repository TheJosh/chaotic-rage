using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.ComponentModel;

namespace Maptool
{
    public class Paint
    {
        private Color color;

        private Bitmap target;
        private Graphics g;
        bool down;


        /**
         * Create the obj
         **/
        public Paint()
        {
            this.color = Color.Black;
            this.target = null;
            this.g = null;
            this.down = false;
        }

        /**
         * The current target for manipulation
         **/
        [Browsable(false)]
        public Bitmap Target
        {
            get { return this.target; }
            set {
                this.target = value;
                if (this.target != null) {
                    this.g = Graphics.FromImage(this.target);
                }
            }
        }


        /**
         * Draw colour
         **/
        public Color Color
        {
            get { return this.color; }
            set { this.color = value; }
        }



        /**
         * Handles mouse-down events
         **/
        public void mouseDown(float x, float y)
        {
            this.down = true;
        }


        /**
         * Handles mouse-move events
         **/
        public void mouseMove(float x, float y)
        {
            if (!this.down) return;

            this.g.DrawEllipse(new Pen(this.color), x, y, 4, 4);
        }


        /**
         * Handles mouse-up events
         **/
        public void mouseUp(float x, float y)
        {
            this.down = false;
        }
    }
}
