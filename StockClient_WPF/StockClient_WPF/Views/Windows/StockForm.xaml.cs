using StockClient_WPF.ViewModels.Windows;
using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace StockClient_WPF.Views.Windows
{
    /// <summary>
    /// StockForm.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class StockForm : Window
    {
        public StockFormViewModel ViewModel { get; }

        public StockForm(StockFormViewModel viewModel)
        {
            ViewModel = viewModel;
            DataContext = this;

            InitializeComponent();

            ViewModel.RequestClose += () => Close();
        }
    }
}
