#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>

#include <cstdint>
#include <chrono>
#include <iostream>
#include <memory>

struct GtpContext
{
  typedef std::shared_ptr<GtpContext> Ptr;

  explicit GtpContext(uint64_t id = 0, uint64_t msisdn = 0, uint64_t imsi = 0, uint64_t imei = 0, std::string text = "")
    : Id(id), Msisdn(msisdn), Imsi(imsi), Imei(imei), Text(text)
  {
  }

  uint64_t Id = 0;
  uint64_t Msisdn = 0;
  uint64_t Imsi = 0;
  uint64_t Imei = 0;
  std::string Text;
};

typedef boost::multi_index::multi_index_container<
  GtpContext::Ptr,
  boost::multi_index::indexed_by<
  boost::multi_index::hashed_unique<boost::multi_index::member<GtpContext, uint64_t, &GtpContext::Id>>,
  boost::multi_index::hashed_non_unique<boost::multi_index::member<GtpContext, uint64_t, &GtpContext::Msisdn>>,
  boost::multi_index::hashed_non_unique<boost::multi_index::member<GtpContext, uint64_t, &GtpContext::Imsi>>,
  boost::multi_index::hashed_non_unique<boost::multi_index::member<GtpContext, uint64_t, &GtpContext::Imei>>
  >
> GtpMiContainer;

void TestBinaryOperation()
{
  const uint64_t a = 1;
  const uint64_t b = 4;
  const uint64_t c = 8;

  uint64_t collection = 0;
  collection |= a;
  collection |= b;
  //collection |= c;

  if (collection & a)
  {
    std::cout << "foun out a" << std::endl;
  }
  if (collection & b)
  {
    std::cout << "foun out b" << std::endl;
  }
  if (collection & c)
  {
    std::cout << "foun out c" << std::endl;
  }
}

int main(int argc, char** argv)
{
  try
  {
    std::cout << "Test started" << std::endl;
    std::chrono::system_clock::time_point timePoint = std::chrono::system_clock::now();

    GtpMiContainer contextList;

    GtpContext::Ptr p = std::make_shared<GtpContext>(10, 79031234000, 250070000000000, 44740251000000);
    //GtpContext p(10, 79031234000, 250070000000000, 44740251000000);
    contextList.emplace(p);

    p = std::make_shared<GtpContext>(11, 79031234001, 250070000000001, 44740251000001);
    contextList.emplace(p);

    //std::cout << contextList.count(10) << '\n';

    //const GtpMiContainer::nth_index<1>::type& index = contextList.get<1>();
    //std::cout << index.count(8) << '\n';

    //std::chrono::microseconds timeInMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - timePoint);
    //std::cout << "Test finish " << timeInMicroseconds.count() << std::endl;

    TestBinaryOperation();
  }
  catch (const std::exception& ex)
  {
    std::cout << "Something went wrong, error: " << ex.what() << std::endl;
  }
  return 0;
}
