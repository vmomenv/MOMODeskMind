#include "petai.h"

PetAI ::PetAI (QObject *parent)
    : QObject{parent}
{
    petGreeting = "你好，有任何问题我都可以帮你解答。";

}
QString PetAI::getPetGreeting()
{
    return petGreeting;
}
QString PetAI::getResponse(const QString &question)
{
    // 这里可以实现更复杂的自然语言处理
    if (question.contains("天气"))
    {
        return "我可以帮你查看天气哦！";
    }
    return "抱歉，我不明白你的问题。";
}
