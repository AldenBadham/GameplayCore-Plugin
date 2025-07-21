// // Licensed under the MIT License. See the LICENSE file in the project root for full license information.


#include "Utils/ErrorFunctionLibrary.h"

#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

void UErrorFunctionLibrary::PopMessageBox(const EAppMsgType::Type Type, const FText Message, const FText Title)
{
	FPlatformMisc::MessageBoxExt(Type, *Message.ToString(), *Title.ToString());
}

void UErrorFunctionLibrary::ShowNotificationInfo(const FText Text, const FText SubText)
{
	FNotificationInfo Info(Text);
	Info.SubText = SubText;
	Info.ExpireDuration = 5.0f;
	Info.Image = FAppStyle::GetBrush(TEXT("Icons.Error"));
	FSlateNotificationManager::Get().AddNotification(Info);
}
