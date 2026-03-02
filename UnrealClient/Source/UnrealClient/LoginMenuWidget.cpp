#include "LoginMenuWidget.h"

#include "Components/Button.h"

void ULoginMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	check(LoginWithDeviceIDButton);
	LoginWithDeviceIDButton->OnClicked.AddDynamic(this, &ULoginMenuWidget::LoginWithDeviceID);
}

void ULoginMenuWidget::LoginWithDeviceID()
{

}
