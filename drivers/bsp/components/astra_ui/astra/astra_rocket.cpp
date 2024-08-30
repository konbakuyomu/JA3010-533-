#include <vector>
#include "astra_rocket.h"
#include "astra_logo.h"
#include "ui/item/page/page.h"
#include "pic.h"

std::vector<std::string> labels = {"前方", "后方", "左方", "右方"};

auto *astraLauncher = new astra::Launcher();
auto *rootPage = new yomu::GammaDashboard(labels, yomu::GammaDashboard::DoseDisplayMode::RealTime);
auto *secondPage = new yomu::GammaDashboard(labels, yomu::GammaDashboard::DoseDisplayMode::Accumulated);
auto *thirdPage = new yomu::PDXDashboard(labels);
auto *treeRootPage = new astra::Tile("树形根目录");
auto *cumulativeDoseThresholdPage = new astra::List("累计剂量阈值", get_pic_cumulative_dose(), astra::List::ItemAction::EnterSubpage);
auto *realtimeDoseThresholdPage = new astra::List("实时剂量阈值", get_pic_realtime_dose(), astra::List::ItemAction::EnterSubpage);
auto *clearCumulativeDosePage = new astra::List("清空累计剂量", get_pic_clear_cumulative(), astra::List::ItemAction::ShowPopup);
auto *buzzerSwitchPage = new astra::List("蜂鸣器开关", get_pic_buzzer() , astra::List::ItemAction::ShowPopup);

void astraCoreInit(void)
{
  HAL::inject(new HALDreamCore);
  HAL::delay(150);
  astra::drawSTART(80);
  // HAL::setFont(astra::getUIConfig().mainFont);

  // 添加环形界面
  rootPage->addMenu(secondPage);
  secondPage->addMenu(thirdPage);
  thirdPage->addMenu(rootPage);

  // 环形界面与树形界面的链接(这里的添加顺序反过来是因为addItem有添加父界面指针的操作)
  thirdPage->addItem(treeRootPage);
  secondPage->addItem(treeRootPage);
  rootPage->addItem(treeRootPage);

  // 添加树形界面
  treeRootPage->addItem(cumulativeDoseThresholdPage);
  treeRootPage->addItem(realtimeDoseThresholdPage);
  treeRootPage->addItem(clearCumulativeDosePage);
  treeRootPage->addItem(buzzerSwitchPage);
  // 树形界面：累计剂量阈值
  cumulativeDoseThresholdPage->addItem(new yomu::NumberEditor("探头1", yomu::NumberEditor::DoseType::Cumulative));
  cumulativeDoseThresholdPage->addItem(new yomu::NumberEditor("探头2", yomu::NumberEditor::DoseType::Cumulative));
  cumulativeDoseThresholdPage->addItem(new yomu::NumberEditor("探头3", yomu::NumberEditor::DoseType::Cumulative));
  cumulativeDoseThresholdPage->addItem(new yomu::NumberEditor("探头4", yomu::NumberEditor::DoseType::Cumulative));
  // 树形界面：实时剂量阈值
  realtimeDoseThresholdPage->addItem(new yomu::NumberEditor("探头1", yomu::NumberEditor::DoseType::RealTime));
  realtimeDoseThresholdPage->addItem(new yomu::NumberEditor("探头2", yomu::NumberEditor::DoseType::RealTime));
  realtimeDoseThresholdPage->addItem(new yomu::NumberEditor("探头3", yomu::NumberEditor::DoseType::RealTime));
  realtimeDoseThresholdPage->addItem(new yomu::NumberEditor("探头4", yomu::NumberEditor::DoseType::RealTime));
  // 树形界面：清空累计剂量
  clearCumulativeDosePage->addItem(new astra::Divider("探头1"));
  clearCumulativeDosePage->addItem(new astra::Divider("探头2"));
  clearCumulativeDosePage->addItem(new astra::Divider("探头3"));
  clearCumulativeDosePage->addItem(new astra::Divider("探头4"));
  // 树形界面：蜂鸣器开关
  buzzerSwitchPage->addItem(new astra::Divider("开启"));
  buzzerSwitchPage->addItem(new astra::Divider("关闭"));

  astraLauncher->init(rootPage);
}

void astraCoreStart(void)
{
  for (;;)
  {
    astraLauncher->update();
    vTaskDelay(5 / portTICK_PERIOD_MS);
  }
}

void astraCoreDestroy(void)
{
  HAL::destroy();
  delete astraLauncher;
}
