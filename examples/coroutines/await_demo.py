import asyncio


async def async_generator():
    for i in range(3):
        await asyncio.sleep(1)
        yield i*i


async def main():
    async for i in async_generator():
        print(i)

if __name__ == "__main__":
    asyncio.run(main())

# loop = asyncio.get_event_loop()
# try:
#     loop.run_until_complete(main())
# finally:
#     loop.run_until_complete(loop.shutdown_asyncgens())  # see: https://docs.python.org/3/library/asyncio-eventloop.html#asyncio.loop.shutdown_asyncgens
#     loop.close()
    
