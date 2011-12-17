using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace datatool
{
    public partial class metric_compare : Form, ListenForm
    {
        private List<base_item> data;

        public metric_compare(List<string> metrics, List<base_item> data)
        {
            InitializeComponent();

            this.data = data;

            metriccombo.Items.Clear();
            foreach (string m in metrics)
            {
                metriccombo.Items.Add(m);
            }
            metriccombo.SelectedIndex = 0;
        }

        private void metric_grid_Load(object sender, EventArgs e)
        {
            ((Main)this.MdiParent).registerListener(this);
            fill_grid();
        }

        private void metric_grid_FormClosed(object sender, FormClosedEventArgs e)
        {
            ((Main)this.MdiParent).unregisterListener(this);
        }

        public void Message_Updated()
        {
            fill_grid();
        }

        private void metriccombo_SelectedIndexChanged(object sender, EventArgs e)
        {
            fill_grid();
        }


        private void fill_grid()
        {
            int metric = metriccombo.SelectedIndex;
            
            datagrid.Columns.Clear();
            datagrid.Columns.Add("", "");
            datagrid.Columns[0].DefaultCellStyle = datagrid.ColumnHeadersDefaultCellStyle;
            foreach (base_item col in this.data)
            {
                datagrid.Columns.Add(col.getName(), col.getName());
            }
            
            foreach (DataGridViewColumn c in datagrid.Columns) {
                c.Width = 75;
            }
            datagrid.Columns[0].Width = 100;

            datagrid.Rows.Clear();
            foreach (base_item row in this.data)
            {
                DataGridViewRow dgr = new DataGridViewRow();
                dgr.CreateCells(datagrid);
                dgr.Cells[0].Value = row.getName();
                
                int i = 1;
                foreach (base_item col in this.data)
                {
                    dgr.Cells[i].Value = col.getMetric(metric) - row.getMetric(metric);
                    i++;
                }

                datagrid.Rows.Add(dgr);
            }
        }
    }
}
