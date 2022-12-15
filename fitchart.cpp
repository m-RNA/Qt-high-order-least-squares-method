#include "fitchart.h"
#include "ui_fitchart.h"
#include <QInputDialog> // // 保留右上角关闭按钮 传参就ok

FitChart::FitChart(QWidget *parent) : QWidget(parent),
									  ui(new Ui::FitChart)
{
	ui->setupUi(this);

	ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
									QCP::iSelectLegend | QCP::iSelectPlottables);
	ui->customPlot->xAxis->setRange(-8, 8);
	ui->customPlot->yAxis->setRange(-5, 5);
	ui->customPlot->axisRect()->setupFullAxesBox();

	ui->customPlot->xAxis->setLabel("x轴");
	ui->customPlot->yAxis->setLabel("y轴");
	ui->customPlot->legend->setVisible(true);

	QFont legendFont = font();
	legendFont.setPointSize(10);
	ui->customPlot->legend->setFont(legendFont);
	ui->customPlot->legend->setSelectedFont(legendFont);
	ui->customPlot->legend->setSelectableParts(QCPLegend::spItems); // 图例框不能选择，只能选择图例项

	QPen pen;
	pen.setColor(Qt::darkGreen); // 设置画笔风格
	pen.setWidth(3);
	pen.setStyle(Qt::PenStyle::DotLine); // 虚线

	QCPGraph *curGraph = ui->customPlot->addGraph();
	curGraph->setName(QString("平均"));
	curGraph->setPen(pen);
	curGraph->setLineStyle(QCPGraph::lsLine);							  // lsNone
	curGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross)); //

	// addRandomGraph();
	ui->customPlot->addGraph();
	ui->customPlot->graph()->setName(QString("拟合"));		 // .arg(ui->customPlot->graphCount()-1) 这个可以添加编号
	ui->customPlot->graph()->setLineStyle(QCPGraph::lsLine); // 连线
	ui->customPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone));
	pen.setColor(Qt ::red);
	pen.setStyle(Qt::PenStyle::SolidLine); // 实线

	ui->customPlot->graph()->setPen(pen);

	// 连接将某些轴选择连接在一起的插槽（尤其是对面的轴）：
	connect(ui->customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));

	// 连接插槽，注意选择轴时，只能拖动和缩放该方向：
	connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent *)), this, SLOT(mousePress()));
	connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent *)), this, SLOT(mouseWheel()));

	// 使底部和左侧轴将其范围镜像到顶部和右侧轴：
	connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
	connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

	// 连接一些交互槽：
	connect(ui->customPlot, SIGNAL(axisDoubleClick(QCPAxis *, QCPAxis::SelectablePart, QMouseEvent *)), this, SLOT(axisLabelDoubleClick(QCPAxis *, QCPAxis::SelectablePart)));
	connect(ui->customPlot, SIGNAL(axisDoubleClick(QCPAxis *, QCPAxis::SelectablePart, QMouseEvent *)), this, SLOT(axisXYDoubleClick(QCPAxis *, QCPAxis::SelectablePart)));
	connect(ui->customPlot, SIGNAL(legendDoubleClick(QCPLegend *, QCPAbstractLegendItem *, QMouseEvent *)), this, SLOT(legendDoubleClick(QCPLegend *, QCPAbstractLegendItem *)));

	// 设置右键菜单弹出窗口的策略和连接槽：
	ui->customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->customPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));
}

FitChart::~FitChart()
{
	delete ui;
}

// 双击坐标标签
void FitChart::axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part)
{
	// 通过双击轴标签来设置轴标签
	if (part == QCPAxis::spAxisLabel) // only react when the actual axis label is clicked, not tick label or axis backbone
									  // 仅在单击实际轴标签时作出反应，而不是勾选标签或轴主干
	{
		bool ok;
		QString newLabel = QInputDialog::getText(this, "重命名", "新的坐标轴名称", QLineEdit::Normal, axis->label(), &ok, Qt::WindowCloseButtonHint);
		if (ok)
		{
			axis->setLabel(newLabel);
			ui->customPlot->replot();
		}
	}
}
// 双击坐标轴
void FitChart::axisXYDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part)
{
	// 通过双击轴来设置轴范围
	if (part == QCPAxis::spAxis)
	{
		bool ok;
		double newRange = QInputDialog::getDouble(this, "设置范围", "新的坐标轴范围", xRange /*axis->range()*/, 0, 99999, 1, &ok, Qt::WindowCloseButtonHint);
		if (newRange > 0.001)
			xRange = newRange;
		if (ok)
		{
			axis->setRange(x_default, xRange, Qt::AlignRight); // 右对齐
			ui->customPlot->replot();
		}
	}
}
// 双击曲线标签
void FitChart::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
{
	// 双击图例项重命名图形
	Q_UNUSED(legend)
	if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
	{		  // 仅当项目被单击时做出反应（用户可以在没有项目的图例的边框填充上单击，则项目为0）
		QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem *>(item);
		bool ok;
		// 记得传参 保留右上角关闭按钮就ok
		QString newName = QInputDialog::getText(this, "重命名", "新的曲线名称", QLineEdit::Normal, plItem->plottable()->name(), &ok, Qt::WindowCloseButtonHint);
		if (ok)
		{
			plItem->plottable()->setName(newName);
			ui->customPlot->replot();
		}
	}
}

void FitChart::selectionChanged()
{
	/*
	通常，轴基线、轴刻度标签和轴标签可以单独选择，但我们希望使用时它两一个整体，
	因此我们将刻度标签的选定状态和轴基线绑定在一起。不过，轴标签应可单独选择。
	左、右轴的选择状态与下、上轴的状态应同步。
	此外，我们希望将图形的选择与属于该图形的相应图例项的选择状态同步。
	因此，用户可以通过单击曲线本身或其图例项来选择图形。
	*/

	// 使上下轴同步选择，并将轴和记号标签作为一个可选对象处理：
	if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
		ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
	{
		ui->customPlot->xAxis2->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
		ui->customPlot->xAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
	}
	// 使左右轴同步选择，并将轴和记号标签作为一个可选对象处理：
	if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
		ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
	{
		ui->customPlot->yAxis2->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
		ui->customPlot->yAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
	}

	// 将曲线的选择与相应图例项的选择同步：
	for (int i = 0; i < ui->customPlot->graphCount(); ++i)
	{
		QCPGraph *graph = ui->customPlot->graph(i);
		QCPPlottableLegendItem *item = ui->customPlot->legend->itemWithPlottable(graph);
		if (item->selected() || graph->selected())
		{
			item->setSelected(true);
			graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
		}
	}
}
// 长按鼠标
void FitChart::mousePress()
{
	// 如果选择了轴，则只允许拖动该轴的方向
	// 如果未选择轴，则可以拖动两个方向

	if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
		ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->xAxis->orientation());
	else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
		ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->yAxis->orientation());
	else
		ui->customPlot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
}
// 鼠标滚轮
void FitChart::mouseWheel()
{
	// 如果选择了轴，则只允许缩放该轴的方向
	// 如果未选择轴，则可以缩放两个方向

	if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
		ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->xAxis->orientation());
	else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
		ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->yAxis->orientation());
	else
		ui->customPlot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
}
// 寻找曲线
void FitChart::findGraph()
{
	ui->customPlot->rescaleAxes(true); // 调整显示区域
	ui->customPlot->replot();		   // 刷新画图
}

// 更新采集的数据曲线
void FitChart::updateCollectPlot(QVector<double> x, QVector<double> y)
{
	qDebug() << "更新采集的数据曲线";
	// 添加数据
	ui->customPlot->graph(0)->setData(x, y);
	// rescalseValueAxis
	ui->customPlot->rescaleAxes(); // 调整显示区域（要画完才调用）只会缩小 不会放大
	// ui->customPlot->xAxis->setRange(x, xRange, Qt::AlignRight); // 曲线能动起来的关键在这里，设定x轴范围为最近xRange个数据 右对齐
	ui->customPlot->replot(); // 刷新画图
}

// 更新拟合曲线
void FitChart::updateFitPlot(QVector<double> x, QVector<double> y)
{
	qDebug() << "更新拟合曲线";
	// 添加数据
	ui->customPlot->graph(1)->setData(x, y);
	// rescalseValueAxis
	// ui->customPlot->rescaleAxes(); // 调整显示区域（要画完才调用）只会缩小 不会放大
	ui->customPlot->replot(); // 刷新画图
}

void FitChart::addVLine(double x)
{
}

void FitChart::addHLine(double y)
{
}
// 清空图线
void FitChart::clear()
{
	ui->customPlot->graph(0)->data()->clear();
	ui->customPlot->graph(1)->data()->clear();
	ui->customPlot->replot();
}

// 隐藏采集的数据曲线
void FitChart::hideCollectPlot()
{
	ui->customPlot->graph(0)->setVisible(false);
	ui->customPlot->replot();
}

// 显示采集的数据曲线
void FitChart::showCollectPlot()
{
	ui->customPlot->graph(0)->setVisible(true);
	ui->customPlot->replot();
}

// 隐藏拟合曲线
void FitChart::hideFitPlot()
{
	ui->customPlot->graph(1)->setVisible(false);
	ui->customPlot->replot();
}

// 显示拟合曲线
void FitChart::showFitPlot()
{
	ui->customPlot->graph(1)->setVisible(true);
	ui->customPlot->replot();
}

// 右键菜单
void FitChart::contextMenuRequest(QPoint pos)
{
	QMenu *menu = new QMenu(this);
	menu->setAttribute(Qt::WA_DeleteOnClose);

	if (ui->customPlot->legend->selectTest(pos, false) >= 0) // 请求曲线标签上的右键菜单
	{
		menu->addAction("移到左上角", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop | Qt::AlignLeft));
		menu->addAction("移到正上方", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop | Qt::AlignHCenter));
		menu->addAction("移到右上角", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop | Qt::AlignRight));
		menu->addAction("移到右下角", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom | Qt::AlignRight));
		menu->addAction("移到左下角", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom | Qt::AlignLeft));
	}
	else // 请求的图形上的通用右键菜单
	{
		menu->addAction("适应图线范围", this, &FitChart::findGraph);
		// menu->addAction("清空绘图", this, &FitChart::clear);
		if (ui->customPlot->graph(0)->visible())
			menu->addAction("隐藏采集数据", this, &FitChart::hideCollectPlot);
		else
			menu->addAction("显示采集数据", this, &FitChart::showCollectPlot);

		if (ui->customPlot->graph(1)->visible())
			menu->addAction("隐藏拟合曲线", this, &FitChart::hideFitPlot);
		else
			menu->addAction("显示拟合曲线", this, &FitChart::showFitPlot);
	}
	menu->popup(ui->customPlot->mapToGlobal(pos));
}

// 移动曲线标签
void FitChart::moveLegend()
{
	if (QAction *contextAction = qobject_cast<QAction *>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
	{																// 确保这个槽真的是由上下文菜单操作调用的，所以它携带了我们需要的数据
		bool ok;
		int dataInt = contextAction->data().toInt(&ok);
		if (ok)
		{
			ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment)dataInt);
			ui->customPlot->replot();
		}
	}
}

void FitChart::graphClicked(QCPAbstractPlottable *plottable, int dataIndex)
{
	// since we know we only have QCPGraphs in the plot, we can immediately access interface1D()
	// usually it's better to first check whether interface1D() returns non-zero, and only then use it.
	// 因为我们知道绘图中只有QCPGraphs，所以我们可以立即访问interface1D（）
	// 通常最好先检查interface1D（）是否返回非零，然后才使用它。

	double dataValue = plottable->interface1D()->dataMainValue(dataIndex);
	QString message = QString("Clicked on graph '%1' at data point #%2 with value %3.").arg(plottable->name()).arg(dataIndex).arg(dataValue);
	// ui->statusBar->showMessage(message, 2500);
}
