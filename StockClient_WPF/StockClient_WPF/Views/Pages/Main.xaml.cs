using StockClient_WPF.ViewModels.Pages;
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
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace StockClient_WPF.Views.Pages
{
    /// <summary>
    /// Main.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class Main : Page
    {
        public MainViewModel ViewModel { get; }

        public Main(MainViewModel viewModel)
        {
            this.ViewModel = viewModel;

            InitializeComponent();
        }

    }
}
