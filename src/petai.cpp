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
