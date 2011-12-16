using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace datatool
{
    public partial class metric_grid : Form, ListenForm
    {
        private List<base_item> data;
        int num_metrics;

        public metric_grid(List<string> metrics, List<base_item> data)
        {
            InitializeComponent();

            this.data = data;
            this.num_metrics = metrics.Count;

            datagrid.Columns.Clear();
            datagrid.Columns.Add("Item", "Item");
            foreach (string m in metrics) {
                datagrid.Columns.Add(m, m);
            }
        }

        private void metric_grid_Load(object sender, EventArgs e)
        {
            ((Main)this.MdiParent).registerListener(this);
            fill_grid();
        }

        public void Message_Updated()
        {
            fill_grid();
        }

        private void fill_grid()
        {
            datagrid.Rows.Clear();

            foreach (base_item item in this.data) {
                DataGridViewRow row = new DataGridViewRow();

                row.CreateCells(datagrid);
                row.Cells[0].Value = item.getName();

                for (int i = 0; i < this.num_metrics; i++) {
                    row.Cells[i+1].Value = item.getMetric(i);
                }

                datagrid.Rows.Add(row);
            }
        }

        private void metric_grid_FormClosed(object sender, FormClosedEventArgs e)
        {
            ((Main)this.MdiParent).unregisterListener(this);
        }
    }
}
